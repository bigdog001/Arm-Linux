#include <stdio.h>
#include "serial.h"
#include "adc_ts.h"


int main()
{
    char c;
    
    uart0_init();   // ������115200��8N1(8������λ����У��λ��1��ֹͣλ)
    
    while (1)
    {
        printf("\r\n##### Test ADC and Touch Screem #####\r\n");
        printf("[A] Test ADC\n\r");
        printf("[T] Test Touch Screem\n\r");
        printf("Enter your selection: ");

        c = getc();
        printf("%c\n\r", c);
        switch (c)
        {
            case 'a':
            case 'A':
            {
                Test_Adc();
                break;
            }
            
            case 't':
            case 'T':
            {
                Test_Ts();
                break;
            }

            default: 
                break;
        }
    }
    
    return 0;
}

