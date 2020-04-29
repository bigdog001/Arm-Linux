
void delay(volatile int d)
{
	while (d--);
}

int led_on(int which)
{
	unsigned int *pGPFCON = (unsigned int *)0x56000050;
	unsigned int *pGPFDAT = (unsigned int *)0x56000054;

	if (which == 4)
	{
		/* ����GPF4Ϊ������� */
		*pGPFCON = 0x100;
	}
	else if (which == 5)
	{
		/* ����GPF5Ϊ������� */
		*pGPFCON = 0x400;
	}
	
	/* ����GPF4/5���0 */
	*pGPFDAT = 0;

	return 0;
}



