#include <config.h>
#include <page_manager.h>

#if 0
/* �˵������� */
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

/* manualҳ���Run��������: 
 * NULL   - ��ʾbrowseҳ��, 
 * ��NULL - �ļ���, ��ʾ��
 */
static void ManualPageRun(void *pVoid)
{
    if (NULL == pVoid)
    {
        /* browseҳ���Run��������: 
         * 0 - ���ڹۿ�ͼƬ
         * 1 - �������/ѡ���ļ���, ����ļ��޷�Ӧ 
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

