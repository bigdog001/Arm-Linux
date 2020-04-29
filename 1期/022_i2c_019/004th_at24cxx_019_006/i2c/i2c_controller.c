
#include "i2c_controller.h"

#define I2C_CONTROLLER_NUM 10

/* ��һ��i2c_controller����������Ÿ��ֲ�ͬоƬ�Ĳ����ṹ�� */
static p_i2c_controller p_i2c_controllers[I2C_CONTROLLER_NUM];
static p_i2c_controller p_i2c_con_selected;


void register_i2c_controller(p_i2c_controller *p)
{
	int i;
	for (i = 0; i < I2C_CONTROLLER_NUM; i++)
	{
		if (!p_i2c_controllers[i])
		{
			p_i2c_controllers[i] = p;
			return;
		}
	}
}

/* ����������ѡ��ĳ��I2C������ */
int select_i2c_controller(char *name)
{
	int i;
	for (i = 0; i < I2C_CONTROLLER_NUM; i++)
	{
		if (p_i2c_controllers[i] && !strcmp(name, p_i2c_controllers[i]->name))
		{
			p_i2c_con_selected = p_i2c_controllers[i];
			return 0;
		}
	}
	return -1;
}

/* ʵ�� i2c_transfer �ӿں��� */

int i2c_transfer(p_i2c_msg msgs, int num)
{
	return p_i2c_con_selected->master_xfer(msgs, num);
}


void i2c_init(void)
{
	/* ע�������I2C������ */
	s3c2440_i2c_con_add();

	/* ѡ��ĳ��I2C������ */
	select_i2c_controller("s3c2440");

	/* ��������init���� */
	p_i2c_con_selected->init();
}


