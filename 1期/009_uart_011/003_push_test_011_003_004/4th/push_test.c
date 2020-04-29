/*
 *  push_test.c  V1.0 
 *  Copyright (c) 2017 Shenzhen 100ask Technology Co.Ltd.All rights reserved.
 *  http://www.100ask.org
 *  100ask.taobao.com
 *
 *  ����ƽ̨:   ubuntu16.04(64λ����)  gcc -m32 -o push_test  push_test.c 
 *				ubuntu9.10 (32λ����)  gcc      -o push_test  push_test.c 
 *  ������  :   gcc
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
 *����:x86ƽ̨,��������ʱ����������ʹ�ö�ջ��ʵ�ֵ� 
 *Ŀ��:�����д���Ĳ���ȫ����ӡ���� 
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
	
	/*ָ��������ռ����ʱ: 1) ȡֵ  2)�ƶ�ָ��*/  
	//i = *((int *)p);
	//p = p + sizeof(int); 	
	i = va_arg(p,int);
	printf("arg2 : %d\n",i);   

    //==============             
	/*ָ��������ռ����ʱ: 1) ȡֵ  2)�ƶ�ָ��*/    
 	//per = *((struct  person *)p); 
	//p = p + sizeof(struct person);	
	per = va_arg(p,struct person);
	printf("arg3: .name = %s, .age = %d, .socre=%c  .id=%d\n",\
		          per.name,   per.age,   per.score, per.id);   

    //==============    
	/*ָ��������ռ����ʱ: 1) ȡֵ  2)�ƶ�ָ��*/
	//c = *((char *)p);
	//p = p + ((sizeof(char) + 3) & ~3);	
	c = va_arg(p,int);
	printf("arg4: %c\n",c);

    //==============    
	/*ָ��������ռ����ʱ: 1) ȡֵ  2)�ƶ�ָ��*/
	//d = *((double *)p);
	//p = p + sizeof(double);
	d = va_arg(p,double);	
	/*����"Ұָ��"*/
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


