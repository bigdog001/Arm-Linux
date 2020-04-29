#include "i2c_controller.h"
#include "../s3c2440_soc.h"


void i2c_interrupt_func(int irq)
{
	/* 每传输完一个数据将产生一个中断 */

	/* 对于每次传输, 第1个中断是"已经发出了设备地址" */

	
}


void s3c2440_i2c_con_init(void)
{
	/* 设置时钟 */
	/* [7] : IIC-bus acknowledge enable bit, 1-enable in rx mode
	 * [6] : 时钟源, 0: IICCLK = fPCLK /16; 1: IICCLK = fPCLK /512
	 * [5] : 1-enable interrupt
	 * [4] : 读出为1时表示中断发生了, 写入0来清除并恢复I2C操作
	 * [3:0] : Tx clock = IICCLK/(IICCON[3:0]+1).
	 * Tx Clock = 100khz = 50Mhz/16/(IICCON[3:0]+1)
	 */
	IICCON = (0<<6) | (1<<5) | (30<<0);

	/* 注册中断处理函数 */
	register_irq(27, i2c_interrupt_func);
}

void do_master_tx(p_i2c_msg msg)
{
	msg->cnt_transferred = 0;
	
	/* 设置寄存器启动传输 */
	/* 1. 配置为 master tx mode */
		
	/* 2. 把从设备地址写入IICDS */
	IICDS = msg->addr<<1;
	
	/* 3. IICSTAT = 0xf0 , 数据即被发送出去, 将导致中断产生 */
	IICSTAT = 0xf0;
	

	/* 后续的传输由中断驱动 */

	/* 循环等待中断处理完毕 */
	while (msg->cnt_transferred != msg->len);
}

void do_master_rx(p_i2c_msg msg)
{
	msg->cnt_transferred = 0;
	
	/* 设置寄存器启动传输 */
	/* 1. 配置为 Master Rx mode */
		
	/* 2. 把从设备地址写入IICDS */
	IICDS = (msg->addr<<1)|(1<<0);
	
	/* 3. IICSTAT = 0xb0 , 从设备地址即被发送出去, 将导致中断产生 */
	IICSTAT = 0xb0;
	

	/* 后续的传输由中断驱动 */

	/* 循环等待中断处理完毕 */
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

/* 实现i2c_controller
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

