#include "i2c_controller.h"
#include "../s3c2440_soc.h"

static p_i2c_msg p_cur_msg;

void i2c_interrupt_func(int irq)
{
	int index;
	unsigned int iicstat = IICSTAT;

	p_cur_msg->cnt_transferred++;
	
	/* 每传输完一个数据将产生一个中断 */

	/* 对于每次传输, 第1个中断是"已经发出了设备地址" */

	if (p_cur_msg->flags == 0)	/* write */
	{
		/* 对于第1个中断, 它是发送出设备地址后产生的
		 * 需要判断是否有ACK
		 * 有ACK : 设备存在
		 * 无ACK : 无设备, 出错, 直接结束传输
		 */
		if (p_cur_msg->cnt_transferred == 0)  /* 第1次中断 */
		{
			if (iicstat & (1<<0))
			{ /* no ack */
				/* 停止传输 */
				IICSTAT = 0xd0;
				IICCON &= ~(1<<4);
				p_cur_msg->err = -1;
				delay(1000);
				return;
			}
		}

		if (p_cur_msg->cnt_transferred < p_cur_msg->len)
		{
			/* 对于其他中断, 要继续发送下一个数据
			 */
			IICDS = p_cur_msg->buf[p_cur_msg->cnt_transferred];
			IICCON &= ~(1<<4);
		}
		else
		{
			/* 停止传输 */
			IICSTAT = 0xd0;
			IICCON &= ~(1<<4);
			p_cur_msg->err = -1;
			delay(1000);
		}
	}
	else /* read */
	{
		/* 对于第1个中断, 它是发送出设备地址后产生的
		 * 需要判断是否有ACK
		 * 有ACK : 设备存在, 恢复I2C传输, 这样在下一个中断才可以得到第1个数据
		 * 无ACK : 无设备, 出错, 直接结束传输
		 */
		if (p_cur_msg->cnt_transferred == 0)  /* 第1次中断 */
		{
			if (iicstat & (1<<0))
			{ /* no ack */
				/* 停止传输 */
				IICSTAT = 0x90;
				IICCON &= ~(1<<4);
				p_cur_msg->err = -1;
				delay(1000);
				return;
			}
			else  /* ack */
			{
				/* 恢复I2C传输 */
				IICCON &= ~(1<<4);
				return;
			}
		}

		/* 非第1个中断, 表示得到了一个新数据
		 * 从IICDS读出、保存
		 */
		if (p_cur_msg->cnt_transferred < p_cur_msg->len)
		{
			index = p_cur_msg->cnt_transferred - 1;
			p_cur_msg->buf[index] = IICDS;
			/* 恢复I2C传输 */
			IICCON &= ~(1<<4);
		}
		else
		{
			/* 发出停止信号 */
			IICSTAT = 0x90;
			IICCON &= ~(1<<4);
			delay(1000);
		}
	}
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
	IICCON = (1<<7) | (0<<6) | (1<<5) | (30<<0);

	/* 注册中断处理函数 */
	register_irq(27, i2c_interrupt_func);
}

int do_master_tx(p_i2c_msg msg)
{
	p_cur_msg = msg;
	
	msg->cnt_transferred = -1;
	msg->err = 0;
	
	/* 设置寄存器启动传输 */
	/* 1. 配置为 master tx mode */
		
	/* 2. 把从设备地址写入IICDS */
	IICDS = msg->addr<<1;
	
	/* 3. IICSTAT = 0xf0 , 数据即被发送出去, 将导致中断产生 */
	IICSTAT = 0xf0;
	

	/* 后续的传输由中断驱动 */

	/* 循环等待中断处理完毕 */
	while (!msg->err && msg->cnt_transferred != msg->len);

	if (msg->err)
		return -1;
	else
		return 0;
}

int do_master_rx(p_i2c_msg msg)
{
	p_cur_msg = msg;

	msg->cnt_transferred = -1;
	msg->err = 0;
	
	/* 设置寄存器启动传输 */
	/* 1. 配置为 Master Rx mode */
		
	/* 2. 把从设备地址写入IICDS */
	IICDS = (msg->addr<<1)|(1<<0);
	
	/* 3. IICSTAT = 0xb0 , 从设备地址即被发送出去, 将导致中断产生 */
	IICSTAT = 0xb0;
	

	/* 后续的传输由中断驱动 */

	/* 循环等待中断处理完毕 */
	while (!msg->err && msg->cnt_transferred != msg->len);

	if (msg->err)
		return -1;
	else
		return 0;
}

int s3c2440_master_xfer(p_i2c_msg msgs, int num)
{
	int i;
	int err;
	
	for (i = 0; i < num; i++)	
	{
		if (msgs[i]->flags == 0)/* write */
			err = do_master_tx(msgs[i]);
		else
			err = do_master_rx(msgs[i]);
		if (err)
			return err;
	}
	return 0;
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

