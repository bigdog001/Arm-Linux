
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
		
	/* 1. 获得显存 */
	ptVideoMem = GetVideoMem(ID("main"), 1);
	if (ptVideoMem == NULL)
	{
		DBG_PRINTF("can't get video mem for main page!\n");
		return;
	}

	/* 2. 描画数据 */
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

	/* 3. 刷到设备上去 */
	FlushVideoMemToDev(ptVideoMem);

	/* 4. 解放显存 */
	PutVideoMem(ptVideoMem);
}

static void MainPageRun(void)
{
	int iIndex;
	T_InputEvent tInputEvent;
	int bPressed = 0;
	int iIndexPressed = -1;
	
	/* 1. 显示页面 */
	ShowMainPage(g_atMainPageLayout);

	/* 2. 创建Prepare线程 */

	/* 3. 调用GetInputEvent获得输入事件，进而处理 */
	while (1)
	{
		iIndex = MainPageGetInputEvent(g_atMainPageLayout, &tInputEvent);
		if (tInputEvent.iPressure == 0)
		{
			/* 如果是松开 */
			if (bPressed)
			{
				/* 曾经有按钮被按下 */
				ReleaseButton(&g_atMainPageLayout[iIndexPressed]);
				bPressed = 0;
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
					PressButton(&g_atMainPageLayout[iIndexPressed]);
				}
			}
		}		
	}
}


int MainPageGetInputEvent(PT_Layout atLayout, PT_InputEvent ptInputEvent)
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
		if ((tInputEvent.iX >= atLayout[i].iTopLeftX) && (tInputEvent.iX <= atLayout[i].iBotRightX) \
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

int MainPageInit(void)
{
	return RegisterPageAction(&g_tMainPageAction);
}

