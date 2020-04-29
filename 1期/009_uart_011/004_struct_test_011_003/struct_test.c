/*
 *  struct_test.c  V1.0 
 *  Copyright (c) 2017 Shenzhen 100ask Technology Co.Ltd.All rights reserved.
 *  http://www.100ask.org
 *  100ask.taobao.com
 *
 *  ����ƽ̨:   ubuntu16.04(64λ����)  gcc -m32 -o struct_test  struct_test.c 
 *				ubuntu9.10 (32λ����)  gcc      -o struct_test  struct_test.c    
 *  ������  :   gcc
 */
 
#include <stdio.h>
/*
������x86(32λ����)ƽ̨�£�GCC������Ĭ�ϰ�4�ֽڶ��룬	
�磺�ṹ��4�ֽڶ���,���ṹ���Ա�������ڵ��ڴ��ַ��4����������

����ͨ��ʹ��gcc�е�__attribute__ѡ��������ָ���Ķ����С��

1����
__attribute__ ((packed))���������õĽṹ��ȡ���ڱ�������е��Ż����룬
����ʵ��ռ���ֽ������ж��롣

2����
__attribute((aligned (n)))���������õĽṹ���Ա������n�ֽڱ߽��ϡ�
 ����ṹ�����г�Ա�������ֽڳ��ȴ���n��
 ��������Ա�������ֽڳ��������롣

*/
struct  person{
	char *name;
	int  age;
	char score;
	int  id;
};

struct  person1{
	char *name;
	int  age;
	char score;
	int  id;
}__attribute__ ((packed));

struct  person2{
	char *name;
	int  age;
	char score;
	int  id;
}__attribute((aligned (4)));

int main(int argc,char **argv)
{
	struct  person per={"www.100ask.org",10,'A',123};
	printf("sizeof(char   )=%d\n",sizeof(char   ));
	printf("sizeof(int    )=%d\n",sizeof(int    ));
	printf("sizeof(char  *)=%d\n",sizeof(char  *));
	printf("sizeof(char **)=%d\n",sizeof(char **));	
	printf("sizeof(struct  person)=%d\n",sizeof(struct  person));
	printf("sizeof(struct  person1)=%d\n",sizeof(struct  person1));
	printf("sizeof(struct  person2)=%d\n",sizeof(struct  person2));	

	printf("&per.name  =%p,per.name  =%s\n",&per.name ,per.name);
	printf("&per.age   =%p,per.age   =%d\n",&per.age  ,per.age);	
	printf("&per.score =%p,per.score =%d\n",&per.score,per.score);		
	printf("&per.id    =%p,per.id    =%d\n",&per.id   ,per.id);			
	return 0;
}

