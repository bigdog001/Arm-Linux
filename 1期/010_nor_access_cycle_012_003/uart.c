
#include "s3c2440_soc.h"


/* 115200,8n1 */
void uart0_init()
{
	/* �����������ڴ��� */
	/* GPH2,3����TxD0, RxD0 */
	GPHCON &= ~((3<<4) | (3<<6));
	GPHCON |= ((2<<4) | (2<<6));

	GPHUP &= ~((1<<2) | (1<<3));  /* ʹ���ڲ����� */
	

	/* ���ò����� */
	/* UBRDIVn = (int)( UART clock / ( buad rate x 16) ) �C1
	 *  UART clock = 50M
	 *  UBRDIVn = (int)( 50000000 / ( 115200 x 16) ) �C1 = 26
	 */
	UCON0 = 0x00000005; /* PCLK,�ж�/��ѯģʽ */
	UBRDIV0 = 26;

	/* �������ݸ�ʽ */
	ULCON0 = 0x00000003; /* 8n1: 8������λ, �޽���λ, 1��ֹͣλ */

	/*  */

}

int putchar(int c)
{
	/* UTRSTAT0 */
	/* UTXH0 */

	while (!(UTRSTAT0 & (1<<2)));
	UTXH0 = (unsigned char)c;
	
}

int getchar(void)
{
	while (!(UTRSTAT0 & (1<<0)));
	return URXH0;
}

int puts(const char *s)
{
	while (*s)
	{
		putchar(*s);
		s++;
	}
}


