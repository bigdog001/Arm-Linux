#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <file.h>
#include <string.h>


static void AutoPageRun(void)
{
    /* 1. ����һ���߳���������ʾͼƬ */

    /* 2. ��ǰ�̵߳ȴ�����������, �����򵥵�: �������˴�����, ���߳��˳� */
}

static T_PageAction g_tAutoPageAction = {
	.name          = "auto",
	.Run           = AutoPageRun,
};

int AutoPageInit(void)
{
	return RegisterPageAction(&g_tAutoPageAction);
}


