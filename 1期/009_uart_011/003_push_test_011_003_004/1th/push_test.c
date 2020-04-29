/*
 *  push_test.c  V1.0 
 *  Copyright (c) 2017 Shenzhen 100ask Technology Co.Ltd.All rights reserved.
 *  http://www.100ask.org
 *  100ask.taobao.com
 *
 *  ����ƽ̨:   ubuntu16.04(64λ����)   
 *				ubuntu9.10 (32λ����)   
 *  ������  :   gcc
 */
 
#include <stdio.h>

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
	char *p = (char *)&format;
	int i;
	struct  person per;  
	char c;
	double d;
	
	printf("arg1 : %s\n",format);	 

    //==============
	/*ָ��������ռ����ʱ: 1) ȡֵ  2)�ƶ�ָ��*/  
	p = p + sizeof(char *);
	i = *((int *)p);
	printf("arg2 : %d\n",i);   

    //==============             
	/*ָ��������ռ����ʱ: 1) ȡֵ  2)�ƶ�ָ��*/    
	p = p + sizeof(int);  
	per = *((struct  person *)p);  
	printf("arg3: .name = %s, .age = %d, .socre=%c  .id=%d\n",\
		          per.name,   per.age,   per.score, per.id);   

    //==============    
	/*ָ��������ռ����ʱ: 1) ȡֵ  2)�ƶ�ָ��*/
	p = p + sizeof(struct person);
	c = *((char *)p);
	printf("arg4: %c\n",c);

    //==============    
	/*ָ��������ռ����ʱ: 1) ȡֵ  2)�ƶ�ָ��*/
	p = p + ((sizeof(char) + 3) & ~3);
	d = *((double *)p);
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


