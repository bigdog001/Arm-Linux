
/*
 *  printf_test.c  V1.0 
 *  Copyright (c) 2017 Shenzhen 100ask Technology Co.Ltd.All rights reserved.
 *  http://www.100ask.org
 *  100ask.taobao.com
 *
 *  测试平台:   ubuntu16.04(64位机器)   
 *				ubuntu9.10 (32位机器)   
 *  编译器  :   gcc
 */
 
#include <stdio.h>

void test0()
{
	char c;
	char *pc;

	/*第一步 : 所有变量都保存在内存中，我们打印一下变量的存储地址*/
	printf("&c  =%p\n",&c);
	printf("&pc =%p\n",&pc);

	/*第二步：所有变量都可以保存某些值,接着赋值并打印*/
	c  = 'A';
	pc = &c;
	printf("c  =%c\n",c);
	printf("pc =%p\n",pc);

	/*第三步：使用指针：1)取值  2)移动指针*/
	printf("*pc =%c\n",*pc);	
	printf("//=================\n");			
}

void test1()
{
	int  ia;
	int  *pi;
	char *pc;

	/*第一步 : 所有变量都保存在内存中，我们打印一下变量的存储地址*/
	printf("&ia =%p\n",&ia);
	printf("&pi =%p\n",&pi);	
	printf("&pc =%p\n",&pc);		

	/*第二步：所有变量都可以保存某些值,接着赋值并打印*/	
	ia = 0x12345678;
	pi = &ia;
	pc = (char *)&ia;
	printf("ia =0x%x\n",ia);	
	printf("pi =%p\n",pi);		
	printf("pc =%p\n",pc);

	/*第三步：使用指针：1)取值  2)移动指针*/
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


