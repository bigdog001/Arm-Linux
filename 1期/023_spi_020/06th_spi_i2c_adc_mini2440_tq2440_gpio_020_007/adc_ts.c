/*
 * FILE: adc_ts.c
 * ADC�ʹ������Ĳ��Ժ���
 */

#include <stdio.h>
#include "adc_ts.h"
#include "s3c24xx.h"
#include "serial.h"
#include "oled.h"

// ADCCON�Ĵ���
#define PRESCALE_DIS        (0 << 14)
#define PRESCALE_EN         (1 << 14)
#define PRSCVL(x)           ((x) << 6)
#define ADC_INPUT(x)        ((x) << 3)
#define ADC_START           (1 << 0)
#define ADC_ENDCVT          (1 << 15)

// ADCTSC�Ĵ���
#define UD_SEN          (1 << 8)
#define DOWN_INT        (UD_SEN*0)
#define UP_INT          (UD_SEN*1)
#define YM_SEN          (1 << 7)
#define YM_HIZ          (YM_SEN*0)
#define YM_GND          (YM_SEN*1)
#define YP_SEN          (1 << 6)
#define YP_EXTVLT       (YP_SEN*0)
#define YP_AIN          (YP_SEN*1)
#define XM_SEN          (1 << 5)
#define XM_HIZ          (XM_SEN*0)
#define XM_GND          (XM_SEN*1)
#define XP_SEN          (1 << 4)
#define XP_EXTVLT       (XP_SEN*0)
#define XP_AIN          (XP_SEN*1)
#define XP_PULL_UP      (1 << 3)
#define XP_PULL_UP_EN   (XP_PULL_UP*0)
#define XP_PULL_UP_DIS  (XP_PULL_UP*1)
#define AUTO_PST        (1 << 2)
#define CONVERT_MAN     (AUTO_PST*0)
#define CONVERT_AUTO    (AUTO_PST*1)
#define XP_PST(x)       (x << 0)

#define NOP_MODE        0
#define X_AXIS_MODE     1
#define Y_AXIS_MODE     2
#define WAIT_INT_MODE   3


/* ���ý���ȴ��ж�ģʽ��XP_PU,XP_Dis,XM_Dis,YP_Dis,YM_En
 * (1)����S3C2410��λ[8]ֻ��Ϊ0������ֻ��ʹ�������wait_down_int��
 *    ���ȵȴ�Pen Down�жϣ�Ҳ�ȴ�Pen Up�ж�
 * (2)����S3C2440��λ[8]Ϊ0��1ʱ�ֱ��ʾ�ȴ�Pen Down�жϻ�Pen Up�ж�
 */
/* ����"�ȴ��ж�ģʽ"���ȴ������������� */
#define wait_down_int() { ADCTSC = DOWN_INT | XP_PULL_UP_EN | \
                          XP_AIN | XM_HIZ | YP_AIN | YM_GND | \
                          XP_PST(WAIT_INT_MODE); }
/* ����"�ȴ��ж�ģʽ"���ȴ����������ɿ� */
#define wait_up_int()   { ADCTSC = UP_INT | XP_PULL_UP_EN | XP_AIN | XM_HIZ | \
                          YP_AIN | YM_GND | XP_PST(WAIT_INT_MODE); }

/* �����Զ�(����) X/Y������ת��ģʽ */
#define mode_auto_xy()  { ADCTSC = CONVERT_AUTO | XP_PULL_UP_DIS | XP_PST(NOP_MODE); }


extern void (*isr_handle_array[50])(void);

/* 
 * ʹ�ò�ѯ��ʽ��ȡA/Dת��ֵ
 * ���������
 *     ch: ģ���ź�ͨ����ȡֵΪ0~7
 */       
static int ReadAdc(int ch)
{
    // ѡ��ģ��ͨ����ʹ��Ԥ��Ƶ���ܣ�����A/Dת������ʱ�� = PCLK/(49+1)
    ADCCON = PRESCALE_EN | PRSCVL(49) | ADC_INPUT(ch);

    // ���λ[2]����Ϊ��ͨת��ģʽ
    ADCTSC &= ~(1<<2);

    // ����λ[0]Ϊ1������A/Dת��
    ADCCON |= ADC_START;

    // ��A/Dת��������ʼʱ��λ[0]���Զ���0
    while (ADCCON & ADC_START);

    // ���λ[15]������Ϊ1ʱ��ʾת������
    while (!(ADCCON & ADC_ENDCVT));

    // ��ȡ����    
    return (ADCDAT0 & 0x3ff);
}

/* 
 * ����ADC
 * ͨ��A/Dת���������ɱ�������ĵ�ѹֵ
 */       
void Test_Adc(void)
{
    float vol0, vol1;
    int t0, t1;
    char buf[100];

    printf("Measuring the voltage of AIN0 and AIN1, press any key to exit\n\r");
    while (!awaitkey(0))    // ���������룬�򲻶ϲ���
    {
        vol0 = ((float)ReadAdc(0)*3.3)/1024.0;  // �����ѹֵ
        vol1 = ((float)ReadAdc(2)*3.3)/1024.0;  // �����ѹֵ
        t0   = (vol0 - (int)vol0) * 1000;   // ����С������, �������е�printf�޷���ӡ������
        t1   = (vol1 - (int)vol1) * 1000;   // ����С������,  �������е�printf�޷���ӡ������
        printf("AIN0 = %d.%-3dV    AIN2 = %d.%-3dV\r", (int)vol0, t0, (int)vol1, t1);

        sprintf(buf,"ADC: %d.%-3d, %d.%-3d", (int)vol0, t0, (int)vol1, t1);
        
        OLEDPrint(6, 0, buf);
    }
    printf("\n");
}

/* 
 * INT_TC���жϷ������
 * ��������������ʱ�������Զ�(����) X/Y������ת��ģʽ��
 * �����������ɿ�ʱ������ȴ��ж�ģʽ���ٴεȴ�INT_TC�ж�
 */       
static void Isr_Tc(void)
{
    if (ADCDAT0 & 0x8000)
    {
        printf("Stylus Up!!\n\r");
        wait_down_int();    /* ����"�ȴ��ж�ģʽ"���ȴ������������� */
    }
    else 
    {
        printf("Stylus Down: ");

        mode_auto_xy();     /* �����Զ�(����) X/Y������ת��ģʽ */
    
        /* ����λ[0]Ϊ1������A/Dת��
         * ע�⣺ADCDLYΪ50000��PCLK = 50MHz��
         *       Ҫ����(1/50MHz)*50000=1ms֮��ſ�ʼת��X����
         *       �پ���1ms֮��ſ�ʼת��Y����
         */
        ADCCON |= ADC_START;
    }
    
    // ��INT_TC�ж�
    SUBSRCPND |= BIT_SUB_TC;
    SRCPND    |= BIT_ADC;
    INTPND    |= BIT_ADC;
}


/* 
 * INT_ADC���жϷ������
 * A/Dת������ʱ�������ж�
 * �ȶ�ȡX��Y����ֵ���ٽ���ȴ��ж�ģʽ
 */       
static void Isr_Adc(void)
{
    // ��ӡX��Y����ֵ    
    printf("xdata = %4d, ydata = %4d\r\n", (int)(ADCDAT0 & 0x3ff), (int)(ADCDAT1 & 0x3ff));

    /* �ж���S3C2410����S3C2440 */
    if ((GSTATUS1 == 0x32410000) || (GSTATUS1 == 0x32410002))
    {   // S3C2410
        wait_down_int();    /* ����"�ȴ��ж�ģʽ"���ȴ����������ɿ� */
    }
    else
    {   // S3C2440
        wait_up_int();      /* ����"�ȴ��ж�ģʽ"���ȴ����������ɿ� */
    }

    // ��INT_ADC�ж�
    SUBSRCPND |= BIT_SUB_ADC;
    SRCPND    |= BIT_ADC;
    INTPND    |= BIT_ADC;
}

/* 
 * ADC�����������жϷ������
 * ����INT_TC��INT_ADC�жϣ��ֱ�������ǵĴ������
 */       
void AdcTsIntHandle(void)
{
    if (SUBSRCPND & BIT_SUB_TC)
        Isr_Tc();

    if (SUBSRCPND & BIT_SUB_ADC)
        Isr_Adc();
}

/* 
 * ���Դ���������ӡ��������
 */       
void Test_Ts(void)
{
    isr_handle_array[ISR_ADC_OFT] = AdcTsIntHandle;    // ����ADC�жϷ������
    INTMSK &= ~BIT_ADC;          // ����ADC���ж�
    INTSUBMSK &= ~(BIT_SUB_TC);  // ����INT_TC�жϣ��������������»��ɿ�ʱ�����ж�
    INTSUBMSK &= ~(BIT_SUB_ADC); // ����INT_ADC�жϣ���A/Dת������ʱ�����ж�
    
    // ʹ��Ԥ��Ƶ���ܣ�����A/Dת������ʱ�� = PCLK/(49+1)
    ADCCON = PRESCALE_EN | PRSCVL(49);

    /* ������ʱʱ�� = (1/3.6864M)*50000 = 13.56ms
     * �����´��������ٹ�13.56ms�Ų���
     */
    ADCDLY = 50000;

    wait_down_int();    /* ����"�ȴ��ж�ģʽ"���ȴ������������� */

    printf("Touch the screem to test, press any key to exit\n\r");    
    getc();

    // ����ADC�ж�
    INTSUBMSK |= BIT_SUB_TC;
    INTSUBMSK |= BIT_SUB_ADC;
    INTMSK |= BIT_ADC;
}

