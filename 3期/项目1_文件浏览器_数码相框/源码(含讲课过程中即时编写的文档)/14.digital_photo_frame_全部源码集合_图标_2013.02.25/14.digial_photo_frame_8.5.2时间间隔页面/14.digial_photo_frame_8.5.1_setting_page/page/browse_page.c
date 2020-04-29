#include <config.h>
#include <page_manager.h>

static T_PageAction g_tBrowsePageAction = {
	.name          = "browse",
	.Run           = BrowsePageRun,
	.GetInputEvent = BrowsePageGetInputEvent,
	.Prepare       = BrowsePagePrepare;
};

static void BrowsePageRun(void)
{
	/* 1. ��ʾҳ�� */

	/* 2. ����Prepare�߳� */

	/* 3. ����GetInputEvent��������¼����������� */
	while (1)
	{
		InputEvent = BrowsePageGetInputEvent();
		switch (InputEvent)	
		{
			case "����":
			{
				return 0;
			}
			case "��С":
			{
				/* ��ʾ��С��ҳ�� */
				break;
			}
			case "�Ŵ�":
			{
				/* ��ʾ�Ŵ��ҳ�� */
				break;
			}		

			case "�Ϸ�":
			{
				/* ��ʾ��һ��ͼƬ */
				break;
			}		
			case "�·�":
			{
				/* ��ʾ��һ��ͼƬ */
				break;
			}		
			
			case "����":
			{
				Page("auto")->Run();
				break;
			}		

			case "��ס����":
			{
				/* ��ʾŲ����ͼƬ */
				break;
			}
		}
	}
}


int BrowsePageInit(void)
{
	return RegisterPageAction(&g_tBrowsePageAction);
}


