/*---------------------------------------------------------------------*/
/* --- STC MCU International Limited ----------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ------------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.GXWMCU.com --------------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* ���Ҫ�ڳ�����ʹ�ô˴���,���ڳ�����ע��ʹ���˺꾧�Ƽ������ϼ�����   */
/*---------------------------------------------------------------------*/

#include	"config.h"
#include	"GPIO.h"
#include	"delay.h"

/*************	����˵��	**************

����ʹ��P4.7 P4.6 P1.6 P1.7 ����ʾ����ƣ������������

******************************************/

/*************	���س�������	**************/


/*************	���ر�������	**************/


/*************	���غ�������	**************/



/*************  �ⲿ�����ͱ������� *****************/



/******************** IO���ú��� **************************/
void	GPIO_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;		//�ṹ����
	GPIO_InitStructure.Pin  = GPIO_Pin_2 | GPIO_Pin_4;
	GPIO_InitStructure.Mode = GPIO_OUT_PP;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	//��ʼ��

	GPIO_InitStructure.Pin  = GPIO_Pin_2;		//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7, �����
	GPIO_InitStructure.Mode = GPIO_HighZ;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//��ʼ��

	GPIO_InitStructure.Pin  = GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_5;		//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7, �����
	GPIO_InitStructure.Mode = GPIO_HighZ;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	//��ʼ��

	GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1;		//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7, �����
	GPIO_InitStructure.Mode = GPIO_HighZ;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);	//��ʼ��

	GPIO_InitStructure.Pin  = GPIO_Pin_3 | GPIO_Pin_7 ;		//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7, �����
	GPIO_InitStructure.Mode = GPIO_HighZ;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	//��ʼ��

	GPIO_InitStructure.Pin  = GPIO_Pin_4 | GPIO_Pin_5;		//ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7, �����
	GPIO_InitStructure.Mode = GPIO_HighZ;		//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P5,&GPIO_InitStructure);	//��ʼ��


}



/******************** ������ **************************/



#define LED P14


void exint0() interrupt 0
{

}

void main(void)
{
	GPIO_config();

	P32 = 1;  /* 51��Ƭ����, ��������Ҫ��д��1���ܶ�������״̬ */

	IT0 = 0;  /* ����INT0Ϊ�����ء��½��ش��� */
	EX0 = 1;  /* ʹ��INT0 */
	EA = 1;	  /* ���ж� */

	while (1)
	{
		if (P32 == 1)
		{
			LED = 1;
		}
		else
		{
			LED = 0;
		}

		PCON |= 0x02;	   /* MCU�������ģʽ */
		_nop_();
		_nop_();
		

		delay_ms(20);  /* �ӳ�һ��,��Ϊ�õ��ǻ�е����,���� */
	}
}



