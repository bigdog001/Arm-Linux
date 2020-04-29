#include "s3c24xx.h"

void EINT_Handle()
{
    unsigned long oft = INTOFFSET;
    unsigned long val;

	/*
	 * K1,K2,K3,K4��ӦGPF1��GPF4��GPF2��GPF0
	 *            �� EINT1, ETIN4, EINT2, EINT0
	 *            oftΪ 1, 4, 2, 0 (��ӦINTMSK�Ĵ���)
	 */
    
    switch( oft )
    {
        // K1������
        case 1: 
        {   
            GPBDAT |= (0xF<<5);   // ����LEDϨ��
            GPBDAT &= ~(1<<5);      // LED1����
            break;
        }
        
        // K2������
        case 4:
        {   
            GPBDAT |= (0xF<<5);   // ����LEDϨ��
            GPBDAT &= ~(1<<6);      // LED2����
            break;
        }

        // K3������
        case 2:
        {   
            GPBDAT |= (0xF<<5);   // ����LEDϨ��
            GPBDAT &= ~(1<<7);      // LED3����
            break;
        }

        // K4������
        case 0:
        {   
            GPBDAT |= (0xF<<5);   // ����LEDϨ��
            GPBDAT &= ~(1<<8);      // LED4����
            break;
        }

        default:
            break;
    }

    //���ж�
    if( oft == 4 ) 
        EINTPEND = (1<<4);   // EINT4_7����IRQ4
    SRCPND = 1<<oft;
    INTPND = 1<<oft;
}
