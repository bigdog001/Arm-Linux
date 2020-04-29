#include "s3c24xx.h"

void EINT_Handle()
{
    unsigned long oft = INTOFFSET;
    unsigned long val;

	/*
	 * K1,K2,K3,K4对应GPF1、GPF4、GPF2、GPF0
	 *            即 EINT1, ETIN4, EINT2, EINT0
	 *            oft为 1, 4, 2, 0 (对应INTMSK寄存器)
	 */
    
    switch( oft )
    {
        // K1被按下
        case 1: 
        {   
            GPBDAT |= (0xF<<5);   // 所有LED熄灭
            GPBDAT &= ~(1<<5);      // LED1点亮
            break;
        }
        
        // K2被按下
        case 4:
        {   
            GPBDAT |= (0xF<<5);   // 所有LED熄灭
            GPBDAT &= ~(1<<6);      // LED2点亮
            break;
        }

        // K3被按下
        case 2:
        {   
            GPBDAT |= (0xF<<5);   // 所有LED熄灭
            GPBDAT &= ~(1<<7);      // LED3点亮
            break;
        }

        // K4被按下
        case 0:
        {   
            GPBDAT |= (0xF<<5);   // 所有LED熄灭
            GPBDAT &= ~(1<<8);      // LED4点亮
            break;
        }

        default:
            break;
    }

    //清中断
    if( oft == 4 ) 
        EINTPEND = (1<<4);   // EINT4_7合用IRQ4
    SRCPND = 1<<oft;
    INTPND = 1<<oft;
}
