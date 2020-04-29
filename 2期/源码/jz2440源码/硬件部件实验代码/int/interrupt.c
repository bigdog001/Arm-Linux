#include "s3c24xx.h"

void EINT_Handle()
{
    unsigned long oft = INTOFFSET;
    unsigned long val;
    
    switch( oft )
    {
        // S2������
        case 0: 
        {   
            GPFDAT |= (0x7<<4);   // ����LEDϨ��
            GPFDAT &= ~(1<<4);      // LED1����
            break;
        }
        
        // S3������
        case 2:
        {   
            GPFDAT |= (0x7<<4);   // ����LEDϨ��
            GPFDAT &= ~(1<<5);      // LED2����
            break;
        }

        // K4������
        case 5:
        {   
            GPFDAT |= (0x7<<4);   // ����LEDϨ��
            GPFDAT &= ~(1<<6);      // LED4����                
            break;
        }

        default:
            break;
    }

    //���ж�
    if( oft == 5 ) 
        EINTPEND = (1<<11);   // EINT8_23����IRQ5
    SRCPND = 1<<oft;
    INTPND = 1<<oft;
}
