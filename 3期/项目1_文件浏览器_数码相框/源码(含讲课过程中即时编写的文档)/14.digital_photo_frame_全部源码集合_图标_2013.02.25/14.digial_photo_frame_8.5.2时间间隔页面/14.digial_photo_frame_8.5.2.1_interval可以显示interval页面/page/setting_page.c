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
		
	/* 1. 获得显存 */
	ptVideoMem = GetVideoMem(ID("setting"), 1);
	if (ptVideoMem == NULL)
	{
		DBG_PRINTF("can't get video mem for setting page!\n");
		return;
	}

	/* 2. 描画数据 */
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
			/* 前两个是256x128的图标, 第3个图标是128x128的, 要特殊处理 
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
				/* 下一个要处理的是128x128的图标 */
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

	/* 3. 刷到设备上去 */
	FlushVideoMemToDev(ptVideoMem);

	/* 4. 解放显存 */
	PutVideoMem(ptVideoMem);
}



static int SettingPageGetInputEvent(PT_Layout atLayout, PT_InputEvent ptInputEvent)
{
	T_InputEvent tInputEvent;
	int iRet;
	int i = 0;
	
	/* 获得原始的触摸屏数据 
	 * 它是调用input_manager.c的函数，此函数会让当前线否休眠
	 * 当触摸屏线程获得数据后，会把它唤醒
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
	
	/* 处理数据 */
	/* 确定触点位于哪一个按钮上 */
	while (atLayout[i].strIconName)
	{
		if ((tInputEvent.iX >= atLayout[i].iTopLeftX) && (tInputEvent.iX <= atLayout[i].iBotRightX) && \
			 (tInputEvent.iY >= atLayout[i].iTopLeftY) && (tInputEvent.iY <= atLayout[i].iBotRightY))
		{
			/* 找到了被点中的按钮 */
			return i;
		}
		else
		{
			i++;
		}			
	}

	/* 触点没有落在按钮上 */
	return -1;
}


static void SettingPageRun(void)
{
	int iIndex;
	T_InputEvent tInputEvent;
	int bPressed = 0;
	int iIndexPressed = -1;
	
	/* 1. 显示页面 */
	ShowSettingPage(g_atSettingPageLayout);

	/* 2. 创建Prepare线程 */

	/* 3. 调用GetInputEvent获得输入事件，进而处理 */
	while (1)
	{
		iIndex = SettingPageGetInputEvent(g_atSettingPageLayout, &tInputEvent);
		if (tInputEvent.iPressure == 0)
		{
			/* 如果是松开 */
			if (bPressed)
			{
				/* 曾经有按钮被按下 */
				ReleaseButton(&g_atSettingPageLayout[iIndexPressed]);
				bPressed = 0;

				if (iIndexPressed == iIndex) /* 按下和松开都是同一个按钮 */
				{
					switch (iIndexPressed)
					{
						case 1: /* interval按钮 */
						{
							Page("interval")->Run();
							ShowSettingPage(g_atSettingPageLayout);
							break;
						}
						case 2: /* 返回按钮 */
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
			/* 按下状态 */
			if (iIndex != -1)
			{
				if (!bPressed)
				{
					/* 未曾按下按钮 */
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

