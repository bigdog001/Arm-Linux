#include <config.h>
#include <page_manager.h>

#if 0
/* 菜单的区域 */
static T_Layout g_atManualMenuIconsLayout[] = {
	{0, 0, 0, 0, "return.bmp"},
	{0, 0, 0, 0, "zoomout.bmp"},
	{0, 0, 0, 0, "zoomin.bmp"},
	{0, 0, 0, 0, "pre_pic.bmp"},
    {0, 0, 0, 0, "next_pic.bmp"},
    {0, 0, 0, 0, "continue_mod_small.bmp"},
	{0, 0, 0, 0, NULL},
};

static T_PageLayout g_tManualPageMenuIconsLayout = {
	.iMaxTotalBytes = 0,
	.atLayout       = g_atManualMenuIconsLayout,
};
#endif

/* manual页面的Run参数含义: 
 * NULL   - 显示browse页面, 
 * 非NULL - 文件名, 显示它
 */
static void ManualPageRun(void *pVoid)
{
    if (NULL == pVoid)
    {
        /* browse页面的Run参数含义: 
         * 0 - 用于观看图片
         * 1 - 用于浏览/选择文件夹, 点击文件无反应 
         */
        Page("browse")->Run((void *)0);
    }
}

static T_PageAction g_tManualPageAction = {
	.name          = "manual",
	.Run           = ManualPageRun,
};

int ManualPageInit(void)
{
	return RegisterPageAction(&g_tManualPageAction);
}

