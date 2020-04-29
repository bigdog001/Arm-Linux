
/*
 *  printf_test.c  V1.0 
 *  Copyright (c) 2017 Shenzhen 100ask Technology Co.Ltd.All rights reserved.
 *  http://www.100ask.org
 *  100ask.taobao.com
 *
 *  ����ƽ̨:   ubuntu16.04(64λ����)   
 *				ubuntu9.10 (32λ����)   
 *  ������  :   gcc
 */
 
#include <stdio.h>

void test0()
{
	char c;
	char *pc;

	/*��һ�� : ���б������������ڴ��У����Ǵ�ӡһ�±����Ĵ洢��ַ*/
	printf("&c  =%p\n",&c);
	printf("&pc =%p\n",&pc);

	/*�ڶ��������б��������Ա���ĳЩֵ,���Ÿ�ֵ����ӡ*/
	c  = 'A';
	pc = &c;
	printf("c  =%c\n",c);
	printf("pc =%p\n",pc);

	/*��������ʹ��ָ�룺1)ȡֵ  2)�ƶ�ָ��*/
	printf("*pc =%c\n",*pc);	
	printf("//=================\n");			
}

void test1()
{
	int  ia;
	int  *pi;
	char *pc;

	/*��һ�� : ���б������������ڴ��У����Ǵ�ӡһ�±����Ĵ洢��ַ*/
	printf("&ia =%p\n",&ia);
	printf("&pi =%p\n",&pi);	
	printf("&pc =%p\n",&pc);		

	/*�ڶ��������б��������Ա���ĳЩֵ,���Ÿ�ֵ����ӡ*/	
	ia = 0x12345678;
	pi = &ia;
	pc = (char *)&ia;
	printf("ia =0x%x\n",ia);	
	printf("pi =%p\n",pi);		
	printf("pc =%p\n",pc);

	/*��������ʹ��ָ�룺1)ȡֵ  2)�ƶ�ָ��*/
	printf("*pi =0x%x\n",*pi);			
	printf("pc =%p\t",pc);	printf("*pc =0x%x\n",*pc); pc=pc+1;
	printf("pc =%p\t",pc);	printf("*pc =0x%x\n",*pc); pc=pc+1;
	printf("pc =%p\t",pc);	printf("*pc =0x%x\n",*pc); pc=pc+1;
	printf("pc =%p\t",pc);	printf("*pc =0x%x\n",*pc);	
	printf("//=================\n");
}


int main(int argc,char **argv)
{
	printf("sizeof(char   )=%d\n",sizeof(char   ));
	printf("sizeof(int    )=%d\n",sizeof(int    ));
	printf("sizeof(char  *)=%d\n",sizeof(char  *));
	printf("sizeof(char **)=%d\n",sizeof(char **));	
	printf("//=================\n");
	//test0();
	test1();
	return 0;
}


