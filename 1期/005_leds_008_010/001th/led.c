
void delay(volatile int d)
{
	while (d--);
}

int main(void)
{
	volatile unsigned int *pGPFCON = (volatile unsigned int *)0x56000050;
	volatile unsigned int *pGPFDAT = (volatile unsigned int *)0x56000054;
	int val = 0;  /* val: 0b000, 0b111 */
	int tmp;

	/* 设置GPFCON让GPF4/5/6配置为输出引脚 */
	*pGPFCON &= ~((3<<8) | (3<<10) | (3<<12));
	*pGPFCON |=  ((1<<8) | (1<<10) | (1<<12));

	/* 循环点亮 */
	while (1)
	{
		tmp = ~val;
		tmp &= 7;
		*pGPFDAT &= ~(7<<4);
		*pGPFDAT |= (tmp<<4);
		delay(100000);
		val++;
		if (val == 8)
			val =0;
		
	}

	return 0;
}



