#include "i2c_controller.h"
#include "../s3c2440_soc.h"


void i2c_interrupt_func(int irq)
{
	/* ÿ������һ�����ݽ�����һ���ж� */

	/* ����ÿ�δ���, ��1���ж���"�Ѿ��������豸��ַ" */

	
}


void s3c2440_i2c_con_init(void)
{
	/* ����ʱ�� */
	/* [7] : IIC-bus acknowledge enable bit, 1-enable in rx mode
	 * [6] : ʱ��Դ, 0: IICCLK = fPCLK /16; 1: IICCLK = fPCLK /512
	 * [5] : 1-enable interrupt
	 * [4] : ����Ϊ1ʱ��ʾ�жϷ�����, д��0��������ָ�I2C����
	 * [3:0] : Tx clock = IICCLK/(IICCON[3:0]+1).
	 * Tx Clock = 100khz = 50Mhz/16/(IICCON[3:0]+1)
	 */
	IICCON = (0<<6) | (1<<5) | (30<<0);

	/* ע���жϴ����� */
	register_irq(27, i2c_interrupt_func);
}

void do_master_tx(p_i2c_msg msg)
{
	msg->cnt_transferred = 0;
	
	/* ���üĴ����������� */
	/* 1. ����Ϊ master tx mode */
		
	/* 2. �Ѵ��豸��ַд��IICDS */
	IICDS = msg->addr<<1;
	
	/* 3. IICSTAT = 0xf0 , ���ݼ������ͳ�ȥ, �������жϲ��� */
	IICSTAT = 0xf0;
	

	/* �����Ĵ������ж����� */

	/* ѭ���ȴ��жϴ������ */
	while (msg->cnt_transferred != msg->len);
}

void do_master_rx(p_i2c_msg msg)
{
	msg->cnt_transferred = 0;
	
	/* ���üĴ����������� */
	/* 1. ����Ϊ Master Rx mode */
		
	/* 2. �Ѵ��豸��ַд��IICDS */
	IICDS = (msg->addr<<1)|(1<<0);
	
	/* 3. IICSTAT = 0xb0 , ���豸��ַ�������ͳ�ȥ, �������жϲ��� */
	IICSTAT = 0xb0;
	

	/* �����Ĵ������ж����� */

	/* ѭ���ȴ��жϴ������ */
	while (msg->cnt_transferred != msg->len);
}

int s3c2440_master_xfer(p_i2c_msg msgs, int num)
{
	int i;
	for (i = 0; i < num; i++)	
	{
		if (msgs[i]->flags == 0)/* write */
			do_master_tx(msgs[i]);
		else
			do_master_rx(msgs[i]);
	}
}

/* ʵ��i2c_controller
          .init
          .master_xfer
          .name
 */

static i2c_controller s3c2440_i2c_con = {
	.name = "s3c2440",
	.init = s3c2440_i2c_con_init,
	.master_xfer = s3c2440_master_xfer,
};

void s3c2440_i2c_con_add(void)
{
	register_i2c_controller(&s3c2440_i2c_con);
}

