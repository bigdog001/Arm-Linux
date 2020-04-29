#include <config.h>
#include <page_manager.h>

static T_PageAction g_tExplorePageAction = {
	.name          = "explore",
	.Run           = ExplorePageRun,
	.GetInputEvent = ExplorePageGetInputEvent,
	.Prepare       = ExplorePagePrepare;
};

static void ExplorePageRun(void)
{
	/* 1. ��ʾҳ�� */

	/* 2. ����Prepare�߳� */

	/* 3. ����GetInputEvent��������¼����������� */
	while (1)
	{
		InputEvent = ExplorePageGetInputEvent();
		switch (InputEvent)	
		{
			case "����":
			{
				/* �ж��Ƿ��Ѿ��Ƕ��� */
				if (isTopLevel)
					return 0;
				else
				{
					/* ��ʾ��һ��Ŀ¼��ҳ�� */
				}
				break;
			}
			case "ѡ��":
			{
				if (isSelectDir)
				{
					/* ��ʾ��һ��Ŀ¼ */
				}
				else
				{
					/* ���浱ǰҳ�� */
					StorePage();
					Page("browse")->Run();
					/* �ָ�֮ǰ��ҳ�� */
					RestorePage();
				}
				break;
			}
			case "��ҳ":
			{
				/* ��ʾ��һҳ */
				break;
			}		

			case "��ҳ":
			{
				/* ��ʾ��һҳ */
				break;
			}		
		}
	}
}


int ExplorePageInit(void)
{
	return RegisterPageAction(&g_tExplorePageAction);
}

