/*
 *  printf_test.c  V1.0 
 *  Copyright (c) 2017 Shenzhen 100ask Technology Co.Ltd.All rights reserved.
 *  http://www.100ask.org
 *  100ask.taobao.com
 *
 *  测试平台:   ubuntu16.04(64位机器)  gcc -m32 -o printf_test  printf_test.c 
 *				ubuntu9.10 (32位机器)  gcc      -o printf_test  printf_test.c   
 *  编译器  :   gcc
 */

 #include  <stdio.h>


void printf_test(void)
{
	printf("hello word\n");
	printf("This is www.100ask.org   my_printf test\n") ;	
	printf("test char           =%c,%c\n", 'A','a') ;	
	printf("test decimal number =%d\n",    123456) ;
	printf("test decimal number =%d\n",    -123456) ;	
	printf("test hex     number =0x%x\n",  0x55aa55aa) ;	
	printf("test string         =%s\n",    "www.100ask.org") ;		
}

int main(int argc ,char *argv[])
{
	printf_test();
	return 0;
}

