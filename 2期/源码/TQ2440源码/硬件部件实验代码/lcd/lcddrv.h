/*
 * FILE: lcddrv.h
 * ����LCD����������ɫ��ȵĵײ㺯���ӿ�
 */

#ifndef __LCDDRV_H__
#define __LCDDRV_H__

#include <types.h>

#define LOWER21BITS(n)  ((n) & 0x1fffff)

#define BPPMODE_1BPP    0x8
#define BPPMODE_2BPP    0x9
#define BPPMODE_4BPP    0xA
#define BPPMODE_8BPP    0xB
#define BPPMODE_16BPP   0xC
#define BPPMODE_24BPP   0xD

#define LCDTYPE_TFT     0x3

#define ENVID_DISABLE   0
#define ENVID_ENABLE    1

#define FORMAT8BPP_5551 0
#define FORMAT8BPP_565  1

#define HSYNC_NORM      0
#define HSYNC_INV       1

#define VSYNC_NORM      0
#define VSYNC_INV       1

#define BSWP            1
#define HWSWP           1


/* 
 * TQ2440 4.3Ӣ��LCD�ֲ�ΪTQ043TSCM_V0.1_40P.pdf
 * ��������ڵ�5ҳ
 *
 * WXCAT43-TG6#001_V1.0.pdf��22��23ҳ,�����,�����ļ��Ĳ�����һ����
 * 
 * LCD�ֲ��2440�ֲ�"Figure 15-6. TFT LCD Timing Example"һ�ԱȾ�֪������������
 */

//TFT LCD Panel(480*272)
#define MODE_TFT_8BIT_480272 	 (0x4102)
#define MODE_TFT_16BIT_480272    (0x4104)
#define MODE_TFT_24BIT_480272    (0x4108)

//TFT 480272
#define LCD_XSIZE_TFT_480272 	(480)	
#define LCD_YSIZE_TFT_480272 	(272)

//TFT 480272
#define HOZVAL_TFT_480272	(LCD_XSIZE_TFT_480272-1)
#define LINEVAL_TFT_480272	(LCD_YSIZE_TFT_480272-1)

#define CLKVAL_TFT_480272	(4)  /* LCD�ֲ��ʾDclk=9MHz~15MHz, HCLK=100MHz, Dclk=VCLK=HCLK/[(CLKVAL+1)x2]=100/((4+1)*2)=10MHz */

#define VBPD_480272		((2-1)&0xff)     /* tvb=2 */
#define VFPD_480272		((2-1)&0xff)     /* tvf=2 */
#define VSPW_480272		((10-1) &0x3f)   /* tvp=10 */
#define HBPD_480272		((2-1)&0x7f)     /* thb=2 */
#define HFPD_480272		((2-1)&0xff)     /* thf=2 */
#define HSPW_480272		((41-1)&0xff)    /* Thp=41 */



//TFT LCD Panel(640*480)
#define MODE_TFT_1BIT_640480     (0x4201)
#define MODE_TFT_8BIT_640480     (0x4202)
#define MODE_TFT_16BIT_640480    (0x4204)
#define MODE_TFT_24BIT_640480    (0x4208)

//TFT 640480
#define LCD_XSIZE_TFT_640480 	(640)	
#define LCD_YSIZE_TFT_640480 	(480)

//TFT640480
#define HOZVAL_TFT_640480	(LCD_XSIZE_TFT_640480-1)
#define LINEVAL_TFT_640480	(LCD_YSIZE_TFT_640480-1)

//Timing parameter for V16C6448AB(PRIME VIEW) 
#define VBPD_640480		((33-1)&0xff)
#define VFPD_640480		((10-1)&0xff)
#define VSPW_640480		((2-1) &0x3f)
#define HBPD_640480		((48-1)&0x7f)
#define HFPD_640480		((16-1)&0xff)
#define HSPW_640480		((96-1)&0xff)


#define CLKVAL_TFT_640480 	(1)
    //53.5hz @90Mhz
    //VSYNC,HSYNC should be inverted
    //HBPD=47VCLK,HFPD=15VCLK,HSPW=95VCLK
    //VBPD=32HSYNC,VFPD=9HSYNC,VSPW=1HSYNC

#define LCDFRAMEBUFFER 0x30400000

/*
 * ��ʼ������LCD������
 */
void Lcd_Port_Init(void);

/*
 * ��ʼ��LCD������
 * ���������
 * type: ��ʾģʽ
 *      MODE_TFT_8BIT_640480  : 640*640 8bpp��TFT LCD
 *      MODE_TFT_16BIT_640480 : 640*640 16bpp��TFT LCD
 */
void Tft_Lcd_Init(int type);

/*
 * ���õ�ɫ��
 */
void Lcd_Palette8Bit_Init(void);

/*
 * ����LCD�������Ƿ�����ź�
 * ���������
 * onoff: 
 *      0 : �ر�
 *      1 : ��
 */
void Lcd_EnvidOnOff(int onoff);

/*
 * �����Ƿ����LCD��Դ�����ź�LCD_PWREN
 * ���������
 *     invpwren: 0 - LCD_PWREN��ЧʱΪ��������
 *               1 - LCD_PWREN��ЧʱΪ��ת����
 *     pwren:    0 - LCD_PWREN�����Ч
 *               1 - LCD_PWREN�����Ч
 */
void Lcd_PowerEnable(int invpwren, int pwren);

/*
 * ʹ����ʱ��ɫ��Ĵ��������ɫͼ��
 * ���������
 *     color: ��ɫֵ����ʽΪ0xRRGGBB
 */
void ClearScrWithTmpPlt(UINT32 color);

/*
 * ֹͣʹ����ʱ��ɫ��Ĵ���
 */
void DisableTmpPlt(void);

/*
 * �ı��ɫ��Ϊһ����ɫ
 * ���������
 *     color: ��ɫֵ����ʽΪ0xRRGGBB
 */
void ChangePalette(UINT32 color);


#endif /*__LCDDRV_H__*/
