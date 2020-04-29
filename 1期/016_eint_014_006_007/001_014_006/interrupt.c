

/* SRCPND 用来显示哪个中断产生了, 需要清除对应位
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/* INTMSK 用来屏蔽中断, 1-masked
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/* INTPND 用来显示当前优先级最高的、正在发生的中断, 需要清除对应位
 * bit0-eint0
 * bit2-eint2
 * bit5-eint8_23
 */

/* INTOFFSET : 用来显示INTPND中哪一位被设置为1
 */

/* 初始化中断控制器 */
void interrupt_init(void)
{
	INTMSK &= ~((1<<0) | (1<<2) | (1<<5));
}

/* 初始化按键, 设为中断源 */
void key_eint_init(void)
{
	/* 配置GPIO为中断引脚 */
	GPFCON &= ~((3<<0) | (3<<4));
	GPFCON |= ((2<<0) | (2<<4));   /* S2,S3被配置为中断引脚 */

	GPGCON &= ~((3<<6) | (3<<11));
	GPGCON |= ((2<<6) | (2<<11));   /* S4,S5被配置为中断引脚 */
	

	/* 设置中断触发方式: 双边沿触发 */
	EXTINT0 |= (7<<0) | (7<<8);     /* S2,S3 */
	EXTINT1 |= (7<<12);             /* S4 */
	EXTINT2 |= (7<<12);             /* S5 */

	/* 设置EINTMASK使能eint11,19 */
	EINTMASK &= ~((1<<11) | (1<<19));
}

/* 读EINTPEND分辨率哪个EINT产生(eint4~23)
 * 清除中断时, 写EINTPEND的相应位
 */


