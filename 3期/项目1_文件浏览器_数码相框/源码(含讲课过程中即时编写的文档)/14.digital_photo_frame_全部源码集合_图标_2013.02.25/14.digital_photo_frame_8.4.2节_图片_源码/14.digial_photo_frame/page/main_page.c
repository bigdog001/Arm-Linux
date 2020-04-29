
#include <config.h>
#include <page_manager.h>

static T_PageAction g_tMainPageAction = {
	.name          = "main",
	.Run           = MainPageRun,
	.GetInputEvent = MainPageGetInputEvent,
	.Prepare       = MainPagePrepare;
};

static T_Layout g_atMainPageLayout[] = {
	{0, 0, 0, 0, "browse_mode.bmp"},
	{0, 0, 0, 0, "continue_mod.bmp"},
	{0, 0, 0, 0, "setting.bmp"},
	{0, 0, 0, 0, NULL},
};


static void ShowMainPage(PT_Layout atLayout)
{
	PT_VideoMem ptVideoMem;
	T_PixelDatas tOriginIconPixelDatas;
	T_PixelDatas tIconPixelDatas;
	int iIconWidth;
	int iIconHeight;
	int iIconX;
	int iIconY;
	int iXres, iYres, iBpp;
	int iError;
		
	/* 1. ����Դ� */
	ptVideoMem = GetVideoMem(ID("main"), 1);
	if (ptVideoMem == NULL)
	{
		DBG_PRINTF("can't get video mem for main page!\n");
		return;
	}

	/* 2. �軭���� */
	if (ptVideoMem->ePicState != PS_GENERATED)
	{
		GetDispResolution(&iXres, &iYres, &iBpp);
		iIconHeight = iYres * 2 / 10;
		iIconWidth  = iIconHeight * 2;
		
		iIconX = (iXres - iIconWidth)/2;
		iIconY = iYres / 10;

		tIconPixelDatas.iBpp        = iBpp;
		tIconPixelDatas.iWidth      = iIconWidth;
		tIconPixelDatas.iHeight     = iIconHeight;
		tIconPixelDatas.iLineBytes  = iIconWidth * iBpp / 8;
		tIconPixelDatas.iTotalBytes = tIconPixelDatas.iLineBytes * iIconHeight;
		tIconPixelDatas.aucPixelDatas = malloc(tIconPixelDatas.iTotalBytes);
		if (tIconPixelDatas.aucPixelDatas == NULL)
		{
			free(tIconPixelDatas.aucPixelDatas);
			return;
		}

		while (atLayout->strIconName)
		{
			atLayout->iTopLeftX  = iIconX;
			atLayout->iTopLeftY  = iIconY;
			atLayout->iBotRightX = iIconX + iIconWidth - 1;
			atLayout->iBotRightY = iIconY + iIconHeight - 1;

			iError = GetPixelDatasForIcon(atLayout->strIconName, &tOriginIconPixelDatas);
			if (iError)
			{
				DBG_PRINTF("GetPixelDatasForIcon error!\n");
				return;
			}
			PicZoom(&tOriginIconPixelDatas, &tIconPixelDatas);
			PicMerge(iIconX, iIconY, &tIconPixelDatas, &ptVideoMem->tPixelDatas);
			FreePixelDatasForIcon(&tOriginIconPixelDatas);
			atLayout++;
			iIconY +=  iYres * 3 / 10;
		}
		free(tIconPixelDatas.aucPixelDatas);
		ptVideoMem->ePicState = PS_GENERATED;
	}

	/* 3. ˢ���豸��ȥ */
	FlushVideoMemToDev(ptVideoMem);

	/* 4. ����Դ� */
	PutVideoMem(ptVideoMem);
}

static void MainPageRun(void)
{
	/* 1. ��ʾҳ�� */
	ShowMainPage(g_atMainPageLayout);

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

