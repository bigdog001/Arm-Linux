
#include <config.h>
#include <page_manager.h>

static T_PageAction g_tMainPageAction = {
	.name          = "main",
	.Run           = MainPageRun,
	.GetInputEvent = MainPageGetInputEvent,
	.Prepare       = MainPagePrepare;
};

static void MainPageRun(void)
{
	/* 1. ��ʾҳ�� */

	/* 2. ����Prepare�߳� */

	/* 3. ����GetInputEvent��������¼����������� */
	while (1)
	{
		InputEvent = MainPageGetInputEvent();
		switch (InputEvent)	
		{
			case "���ģʽ":
			{
				/* ���浱ǰҳ�� */
				StorePage();
				
				Page("explore")->Run();
				
				/* �ָ�֮ǰ��ҳ�� */
				RestorePage();
				break;
			}
			case "����ģʽ":
			{
				/* ���浱ǰҳ�� */
				StorePage();

				Page("auto")->Run();
				break;
			}
			case "����":
			{
				/* ���浱ǰҳ�� */
				StorePage();
				Page("setting")->Run();
				/* �ָ�֮ǰ��ҳ�� */
				RestorePage();
				break;
			}		
		}
	}
}


int MainPageGetInputEvent(...)
{
	/* ���ԭʼ�Ĵ��������� 
	 * ���ǵ���input_manager.c�ĺ������˺������õ�ǰ�߷�����
	 * ���������̻߳�����ݺ󣬻��������
	 */
	GetInputEvent();
	
	/* �������� */
}

int MainPageInit(void)
{
	return RegisterPageAction(&g_tMainPageAction);
}

