
#include "s3c24xx.h"
#include "gpio_spi.h"

static void SPIFlash_Set_CS(char val)
{
    if (val)
        GPGDAT |= (1<<2);
    else
        GPGDAT &= ~(1<<2);
}

static void SPIFlashSendAddr(unsigned int addr)
{
    SPISendByte(addr >> 16);
    SPISendByte(addr >> 8);
    SPISendByte(addr & 0xff);
}

/* 
 * 
 */
void SPIFlashReadID(int *pMID, int *pDID)
{
    SPIFlash_Set_CS(0); /* Ñ¡ÖÐSPI FLASH */

    SPISendByte(0x90);

    SPIFlashSendAddr(0);

    *pMID = SPIRecvByte();
    *pDID = SPIRecvByte();

    SPIFlash_Set_CS(1);

}

