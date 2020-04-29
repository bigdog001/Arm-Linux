#include <stdio.h>
#include "serial.h"

void init_irq(void);
void read_wav(unsigned int wav_buf, int *fs, int *channels, int *bits_per_sample, int *wav_size);
void iis_init(int bits_per_sample, int fs);
void iis_start(void);
void iis_stop(void);
void dma_init(unsigned int src, unsigned int len);
void dma_start(void);
void dma_stop(void);
void jz2440_init(void);
void tq2440_init(void);
void mini2440_init(void);
void wm8976_init(void);
void wm8976_set_volume(int volume);
void uda1341_init(void);
void uda1341_set_volume(int volume);



void delay(void)
{
    volatile int i = 1000;
    while (i--);
}


int main()
{
    char c;
    int volume = 50; /* 0~100 */

    unsigned int wav_buf = 0x30000000;
    int fs;
    int channels;
    int bits_per_sample;
    int wav_size;

    void (*set_volume)(int);
    
    uart0_init();   // ������115200��8N1(8������λ����У��λ��1��ֹͣλ)

    init_irq();


    /* 1. ��NAND��WAV�ļ�����SDRAM
     * 2. ���WAV�ļ��ĸ�ʽ: ͨ����,������
     */
    read_wav(wav_buf, &fs, &channels, &bits_per_sample, &wav_size);
    printf("wav format: fs = %d, channels = %d, bits_per_sample = %d, wav_size = %d\n\r", fs, channels, bits_per_sample, wav_size);
    if ((channels != 2) || 
        ((bits_per_sample != 8) && (bits_per_sample != 16)))
    {
        printf("can not support this wav\n\r");
        while (1);
    }
        
    while (1)
    {
menu:        
        /* soc init 
         * ��ʼ��IIS֮�����Ż��codecоƬ����ʱ��
         * codecоƬ���ܱ�ʹ��
         */
        iis_init(bits_per_sample, fs);
        dma_init(wav_buf, wav_size);


        printf("\r\n##### Sound Menu #####\r\n");
        printf("[J] JZ2440\n\r");
        printf("[M] MINI2440\n\r");
        printf("[T] TQ2440\n\r");
        printf("Enter your selection: ");

        c = getc();
        printf("%c\n\r", c);
        switch (c)
        {
            case 'J':
            case 'j':
            {
                /* machine init */
                jz2440_init();
                
                /* codec init */
                wm8976_init();

                set_volume = wm8976_set_volume;
                break;
            }
            
            case 'm':
            case 'M':
            {
                /* machine init */
                mini2440_init();

                /* codec init */
                uda1341_init();

                set_volume = uda1341_set_volume;
                break;
            }

            case 't':
            case 'T':
            {
                /* machine init */
                tq2440_init();

                /* codec init */
                uda1341_init();

                set_volume = uda1341_set_volume;
                break;
            }

            default:
            {
                goto menu;
            }
        }        

        set_volume(volume);
        
        dma_start();
        iis_start();

        printf("\r\n##### Operate Menu #####\r\n");
        printf("[-] volume down\n\r");
        printf("[+] volume up\n\r");
        printf("[Q] Quit\n\r");
        printf("Cur volume: %03d", volume);
        while (1)
        {
            c = getc();
            if (c == 'q' || c == 'Q')
            {
                printf("%c\n\r", c);
                iis_stop();
                dma_stop();
                break;
            }
            else if (c == '-')
            {
                if (volume > 0)
                    volume--;
                set_volume(volume);
            }
            else if (c == '+')
            {
                if (volume < 100)
                    volume++;
                set_volume(volume);
            }
            printf("\b\b\b%03d", volume);
        }
        
            
    }
    
    return 0;
}
