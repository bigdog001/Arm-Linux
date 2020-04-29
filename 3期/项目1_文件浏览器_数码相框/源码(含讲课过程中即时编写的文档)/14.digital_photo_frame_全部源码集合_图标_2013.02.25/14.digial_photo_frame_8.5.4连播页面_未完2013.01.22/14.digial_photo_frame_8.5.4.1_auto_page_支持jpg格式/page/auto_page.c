#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <file.h>
#include <string.h>


static void AutoPageRun(void)
{
    /* 1. 启动一个线程来连续显示图片 */

    /* 2. 当前线程等待触摸屏输入, 先做简单点: 如果点击了触摸屏, 让线程退出 */
}

static T_PageAction g_tAutoPageAction = {
	.name          = "auto",
	.Run           = AutoPageRun,
};

int AutoPageInit(void)
{
	return RegisterPageAction(&g_tAutoPageAction);
}


