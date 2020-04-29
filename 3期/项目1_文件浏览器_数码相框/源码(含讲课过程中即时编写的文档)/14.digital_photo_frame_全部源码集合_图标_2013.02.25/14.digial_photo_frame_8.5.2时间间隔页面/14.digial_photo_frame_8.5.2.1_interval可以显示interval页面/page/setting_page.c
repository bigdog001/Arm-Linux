#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <string.h>

static T_Layout g_atSettingPageLayout[] = {
	{0, 0, 0, 0, "select_fold.bmp"},
	{0, 0, 0, 0, "interval.bmp"},
	{0, 0, 0, 0, "return.bmp"},
	{0, 0, 0, 0, NULL},
};

static void ShowSettingPage(PT_Layout atLayout)
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
	ptVideoMem = GetVideoMem(ID("setting"), 1);
	if (ptVideoMem == NULL)
	{
		DBG_PRINTF("can't get video mem for setting page!\n");
		return;
	}

	/* 2. �軭���� */
	if (ptVideoMem->ePicState != PS_GENERATED)
	{
		ClearVideoMem(ptVideoMem, 0);
		
		GetDispResolution(&iXres, &iYres, &iBpp);
		iIconHeight = iYres * 2 / 10;
		iIconWidth	= iIconHeight * 2;
		
		iIconX = (iXres - iIconWidth)/2;
		iIconY = iYres / 10;

		tIconPixelDatas.iBpp		= iBpp;
		tIconPixelDatas.iWidth		= iIconWidth;
		tIconPixelDatas.iHeight 	= iIconHeight;
		tIconPixelDatas.iLineBytes	= iIconWidth * iBpp / 8;
		tIconPixelDatas.iTotalBytes = tIconPixelDatas.iLineBytes * iIconHeight;
		tIconPixelDatas.aucPixelDatas = malloc(tIconPixelDatas.iTotalBytes);
		if (tIconPixelDatas.aucPixelDatas == NULL)
		{
			free(tIconPixelDatas.aucPixelDatas);
			return;
		}

		while (atLayout->strIconName)
		{
			/* ǰ������256x128��ͼ��, ��3��ͼ����128x128��, Ҫ���⴦�� 
			 * 
			 */
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
			
			if (atLayout->strIconName && strcmp(atLayout->strIconName, "return.bmp") == 0)
			{
				/* ��һ��Ҫ�������128x128��ͼ�� */
				iIconWidth = iIconWidth / 2;
				iIconX = (iXres - iIconWidth) / 2;

				tIconPixelDatas.iBpp		= iBpp;
				tIconPixelDatas.iWidth		= iIconWidth;
				tIconPixelDatas.iHeight 	= iIconHeight;
				tIconPixelDatas.iLineBytes	= iIconWidth * iBpp / 8;
				tIconPixelDatas.iTotalBytes = tIconPixelDatas.iLineBytes * iIconHeight;				
			}
		}
		
		free(tIconPixelDatas.aucPixelDatas);
		ptVideoMem->ePicState = PS_GENERATED;
	}

	/* 3. ˢ���豸��ȥ */
	FlushVideoMemToDev(ptVideoMem);

	/* 4. ����Դ� */
	PutVideoMem(ptVideoMem);
}



static int SettingPageGetInputEvent(PT_Layout atLayout, PT_InputEvent ptInputEvent)
{
	T_InputEvent tInputEvent;
	int iRet;
	int i = 0;
	
	/* ���ԭʼ�Ĵ��������� 
	 * ���ǵ���input_manager.c�ĺ������˺������õ�ǰ�߷�����
	 * ���������̻߳�����ݺ󣬻��������
	 */
	iRet = GetInputEvent(&tInputEvent);
	if (iRet)
	{
		return -1;
	}

	if (tInputEvent.iType != INPUT_TYPE_TOUCHSCREEN)
	{
		return -1;
	}

	*ptInputEvent = tInputEvent;
	
	/* �������� */
	/* ȷ������λ����һ����ť�� */
	while (atLayout[i].strIconName)
	{
		if ((tInputEvent.iX >= atLayout[i].iTopLeftX) && (tInputEvent.iX <= atLayout[i].iBotRightX) && \
			 (tInputEvent.iY >= atLayout[i].iTopLeftY) && (tInputEvent.iY <= atLayout[i].iBotRightY))
		{
			/* �ҵ��˱����еİ�ť */
			return i;
		}
		else
		{
			i++;
		}			
	}

	/* ����û�����ڰ�ť�� */
	return -1;
}


static void SettingPageRun(void)
{
	int iIndex;
	T_InputEvent tInputEvent;
	int bPressed = 0;
	int iIndexPressed = -1;
	
	/* 1. ��ʾҳ�� */
	ShowSettingPage(g_atSettingPageLayout);

	/* 2. ����Prepare�߳� */

	/* 3. ����GetInputEvent��������¼����������� */
	while (1)
	{
		iIndex = SettingPageGetInputEvent(g_atSettingPageLayout, &tInputEvent);
		if (tInputEvent.iPressure == 0)
		{
			/* ������ɿ� */
			if (bPressed)
			{
				/* �����а�ť������ */
				ReleaseButton(&g_atSettingPageLayout[iIndexPressed]);
				bPressed = 0;

				if (iIndexPressed == iIndex) /* ���º��ɿ�����ͬһ����ť */
				{
					switch (iIndexPressed)
					{
						case 1: /* interval��ť */
						{
							Page("interval")->Run();
							ShowSettingPage(g_atSettingPageLayout);
							break;
						}
						case 2: /* ���ذ�ť */
						{
							return;
						}
						default:
						{
							break;
						}
					}
				}

				iIndexPressed = -1;
			}
		}
		else
		{
			/* ����״̬ */
			if (iIndex != -1)
			{
				if (!bPressed)
				{
					/* δ�����°�ť */
					bPressed = 1;
					iIndexPressed = iIndex;
					PressButton(&g_atSettingPageLayout[iIndexPressed]);
				}
			}
		}		
	}	
}

static T_PageAction g_tSettingPageAction = {
	.name          = "setting",
	.Run           = SettingPageRun,
	.GetInputEvent = SettingPageGetInputEvent,
	//.Prepare       = MainPagePrepare;
};

int SettingPageInit(void)
{
	return RegisterPageAction(&g_tSettingPageAction);
}

