/*
 * init.c: 进行一些初始化
 */ 

#include "s3c24xx.h"

/*
 * LED1,LED2,LED4对应GPB5、GPB6、GPB7、GPB8
 */
#define	GPB5_out	(1<<(5*2))
#define	GPB6_out	(1<<(6*2))
#define	GPB7_out	(1<<(7*2))
#define	GPB8_out	(1<<(8*2))

#define	GPB5_msk	(3<<(5*2))
#define	GPB6_msk	(3<<(6*2))
#define	GPB7_msk	(3<<(7*2))
#define	GPB8_msk	(3<<(8*2))

/*
 * K1,K2,K3,K4对应GPF1、GPF4、GPF2、GPF0
 */
#define GPF0_int     (0x2<<(0*2))
#define GPF1_int     (0x2<<(1*2))
#define GPF2_int     (0x2<<(2*2))
#define GPF4_int     (0x2<<(4*2))

#define GPF0_msk    (3<<(0*2))
#define GPF1_msk    (3<<(1*2))
#define GPF2_msk    (3<<(2*2))
#define GPF4_msk    (3<<(4*2))

/*
 * 关闭WATCHDOG，否则CPU会不断重启
 */
void disable_watch_dog(void)
{
    WTCON = 0;  // 关闭WATCHDOG很简单，往这个寄存器写0即可
}

void init_led(void)
{
	// LED1,LED2,LED3,LED4对应的4根引脚设为输出
	GPBCON &= ~(GPB5_msk | GPB6_msk | GPB7_msk | GPB8_msk);
	GPBCON |= GPB5_out | GPB6_out | GPB7_out | GPB8_out;
}

/*
 * 初始化GPIO引脚为外部中断
 * GPIO引脚用作外部中断时，默认为低电平触发、IRQ方式(不用设置INTMOD)
 */ 
void init_irq( )
{
	// K1,K2,K3,K4对应的4根引脚设为中断功能
	GPFCON &= ~(GPF0_msk | GPF1_msk | GPF2_msk | GPF4_msk);
	GPFCON |= GPF0_int | GPF1_int | GPF2_int | GPF4_int;

    // 对于EINT4，需要在EINTMASK寄存器中使能它
    EINTMASK &= ~(1<<4);
            
    /*
     * 设定优先级：
     * ARB_SEL0 = 00b, ARB_MODE0 = 0: REQ1 > REQ2 > REQ3，即EINT0 > EINT1 > EINT2
     * 仲裁器1、6无需设置
     * 最终：
     * EINT0 > EINT1> EINT2 > EINT4 即K4 > K1 > K3 > K2
     */
    PRIORITY = (PRIORITY & ((~0x01) | ~(0x3<<7)));

    // EINT0、EINT1、EINT2、EINT4_7使能
    INTMSK   &= (~(1<<0)) & (~(1<<1)) & (~(1<<2)) & (~(1<<4));
}




