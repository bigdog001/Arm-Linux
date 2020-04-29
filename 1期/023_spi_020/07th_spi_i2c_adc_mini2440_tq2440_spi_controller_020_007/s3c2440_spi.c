
#include "s3c24xx.h"

/* SPI controller */

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
    * GPE11 SPIMISO   
    * GPE12 SPIMOSI   
    * GPE13 SPICLK    
    */
    GPFCON &= ~(3<<(3*2));    
    GPFCON |= (1<<(3*2));    

    GPECON &= ~((3<<(11*2)) | (3<<(12*2)) | (3<<(13*2)));
    GPECON |= ((2<<(11*2)) | (2<<(12*2)) | (2<<(13*2)));
}


void SPISendByte(unsigned char val)
{
    while (!(SPSTA0 & 1));
    SPTDAT0 = val;    
}

unsigned char SPIRecvByte(void)
{
    SPTDAT0 = 0xff;    
    while (!(SPSTA0 & 1));
    return SPRDAT0;    
}


static void SPIControllerInit(void)
{
    /* OLED  : 100ns, 10MHz
    * FLASH : 104MHz
    * 取10MHz
    * 10 = 50 / 2 / (Prescaler value + 1)
    * Prescaler value = 1.5 = 2
    * Baud rate = 50/2/3=8.3MHz
    */
    SPPRE0 = 2;
    SPPRE1 = 2;

    /* [6:5] : 00, polling mode
    * [4]   : 1 = enable 
    * [3]   : 1 = master
    * [2]   : 0
    * [1]   : 0 = format A
    * [0]   : 0 = normal mode
    */
    SPCON0 = (1<<4) | (1<<3);
    SPCON1 = (1<<4) | (1<<3);
    
}

void SPIInit(void)
{
    /* 初始化引脚 */
    SPI_GPIO_Init();

    SPIControllerInit();
}



