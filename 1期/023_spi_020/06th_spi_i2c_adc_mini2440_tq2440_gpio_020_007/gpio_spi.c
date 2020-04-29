#include "s3c24xx.h"

/* 用GPIO模拟SPI */

static void SPI_GPIO_Init(void)
{
    /* GPG1 OLED_CSn output 
    * GPG10 FLASH_CSn output
    */
    GPGCON &= ~((3<<(1*2)) | (3<<(10*2)));
    GPGCON |= (1<<(1*2)) | (1<<(10*2));
    GPGDAT |= (1<<1) | (1<<10);

    /* 
    * GPF3  OLED_DC   output
    * GPE11 SPIMISO   input
    * GPE12 SPIMOSI   output
    * GPE13 SPICLK    output
    */
    GPFCON &= ~(3<<(3*2));    
    GPFCON |= (1<<(3*2));    

    GPECON &= ~((3<<(11*2)) | (3<<(12*2)) | (3<<(13*2)));
    GPECON |= ((1<<(12*2)) | (1<<(13*2)));
}

static void SPI_Set_CLK(char val)
{
    if (val)
        GPEDAT |= (1<<13);
    else
        GPEDAT &= ~(1<<13);
}

static void SPI_Set_DO(char val)
{
    if (val)
        GPEDAT |= (1<<12);
    else
        GPEDAT &= ~(1<<12);
}

static char SPI_Get_DI(void)
{
    if (GPEDAT & (1<<11))
        return 1;
    else 
        return 0;
}

void SPISendByte(unsigned char val)
{
    int i;
    for (i = 0; i < 8; i++)
    {
        SPI_Set_CLK(0);
        SPI_Set_DO(val & 0x80);
        SPI_Set_CLK(1);
        val <<= 1;
    }
    
}

unsigned char SPIRecvByte(void)
{
    int i;
    unsigned char val = 0;
    for (i = 0; i < 8; i++)
    {
        val <<= 1;
        SPI_Set_CLK(0);
        if (SPI_Get_DI())
            val |= 1;
        SPI_Set_CLK(1);
    }
    return val;    
}


void SPIInit(void)
{
    /* 初始化引脚 */
    SPI_GPIO_Init();
}

