


int main()
{
	unsigned int *pGPFCON = (unsigned int *)0x56000050;
	unsigned int *pGPFDAT = (unsigned int *)0x56000054;

	/* ����GPF4Ϊ������� */
	*pGPFCON = 0x100;
	
	/* ����GPF4���0 */
	*pGPFDAT = 0;

	return 0;
}

