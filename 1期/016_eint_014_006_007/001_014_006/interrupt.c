

/* SRCPND ������ʾ�ĸ��жϲ�����, ��Ҫ�����Ӧλ
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/* INTMSK ���������ж�, 1-masked
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/* INTPND ������ʾ��ǰ���ȼ���ߵġ����ڷ������ж�, ��Ҫ�����Ӧλ
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/* INTOFFSET : ������ʾINTPND����һλ������Ϊ1
 */

/* ��ʼ���жϿ����� */
void interrupt_init(void)
{
	INTMSK &= ~((1<<0) | (1<<2) | (1<<5));
}

/* ��ʼ������, ��Ϊ�ж�Դ */
void key_eint_init(void)
{
	/* ����GPIOΪ�ж����� */
	GPFCON &= ~((3<<0) | (3<<4));
	GPFCON |= ((2<<0) | (2<<4));   /* S2,S3������Ϊ�ж����� */

	GPGCON &= ~((3<<6) | (3<<11));
	GPGCON |= ((2<<6) | (2<<11));   /* S4,S5������Ϊ�ж����� */
	

	/* �����жϴ�����ʽ: ˫���ش��� */
	EXTINT0 |= (7<<0) | (7<<8);     /* S2,S3 */
	EXTINT1 |= (7<<12);             /* S4 */
	EXTINT2 |= (7<<12);             /* S5 */

	/* ����EINTMASKʹ��eint11,19 */
	EINTMASK &= ~((1<<11) | (1<<19));
}

/* ��EINTPEND�ֱ����ĸ�EINT����(eint4~23)
 * ����ж�ʱ, дEINTPEND����Ӧλ
 */


