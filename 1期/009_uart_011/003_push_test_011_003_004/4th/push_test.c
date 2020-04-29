/*
 *  push_test.c  V1.0 
 *  Copyright (c) 2017 Shenzhen 100ask Technology Co.Ltd.All rights reserved.
 *  http://www.100ask.org
 *  100ask.taobao.com
 *
 *  测试平台:   ubuntu16.04(64位机器)  gcc -m32 -o push_test  push_test.c 
 *				ubuntu9.10 (32位机器)  gcc      -o push_test  push_test.c 
 *  编译器  :   gcc
 */
 
#include <stdio.h>
//#include <stdarg.h>
typedef char *  va_list;
#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )

#define va_start(ap,v)  ( ap = (va_list)&v + _INTSIZEOF(v) )
//#define va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
//#define va_arg(ap,t)    (ap = ap + _INTSIZEOF(t), *(t *)(ap - _INTSIZEOF(t)))
#define va_arg(ap,t)    (*(t *)(ap = ap + _INTSIZEOF(t), ap - _INTSIZEOF(t)))
#define va_end(ap)      ( ap = (va_list)0 )	

struct  person{
	char *name;
	int  age;
	char score;
	int  id;
};
/* 
 *int printf(const char *format, ...); 
 *依据:x86平台,函数调用时参数传递是使用堆栈来实现的 
 *目的:将所有传入的参数全部打印出来 
 */ 
int push_test(const char *format, ...)
{
	//char *p = (char *)&format;
	int i;
	struct  person per;  
	char c;
	double d;
	va_list p;
	
	printf("arg1 : %s\n",format);	 

    //==============
	//p = p + sizeof(char *);
	va_start(p, format );   
	
	/*指针对连续空间操作时: 1) 取值  2)移动指针*/  
	//i = *((int *)p);
	//p = p + sizeof(int); 	
	i = va_arg(p,int);
	printf("arg2 : %d\n",i);   

    //==============             
	/*指针对连续空间操作时: 1) 取值  2)移动指针*/    
 	//per = *((struct  person *)p); 
	//p = p + sizeof(struct person);	
	per = va_arg(p,struct person);
	printf("arg3: .name = %s, .age = %d, .socre=%c  .id=%d\n",\
		          per.name,   per.age,   per.score, per.id);   

    //==============    
	/*指针对连续空间操作时: 1) 取值  2)移动指针*/
	//c = *((char *)p);
	//p = p + ((sizeof(char) + 3) & ~3);	
	c = va_arg(p,int);
	printf("arg4: %c\n",c);

    //==============    
	/*指针对连续空间操作时: 1) 取值  2)移动指针*/
	//d = *((double *)p);
	//p = p + sizeof(double);
	d = va_arg(p,double);	
	/*避免"野指针"*/
	//p = (char *)0;
	va_end( p ); 
	printf("arg5: %f\n",d);
	
	return 0;
}

int main(int argc,char **argv)
{
	struct  person per={"www.100ask.org",10,'A',123};
	
	printf("sizeof(char   )=%d\n",sizeof(char   ));
	printf("sizeof(int    )=%d\n",sizeof(int    ));
	printf("sizeof(char  *)=%d\n",sizeof(char  *));
	printf("sizeof(char **)=%d\n",sizeof(char **));	
	printf("sizeof(struct  person)=%d\n",sizeof(struct  person));		
	
    //push_test("abcd");
    //push_test("abcd",123);	 
    //push_test("abcd",123,per);  				
    //push_test("abcd",123,per,'c');  	 
    push_test("abcd",123,per,'c',2.79); 	
	 		
	return 0;
}	


