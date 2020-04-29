
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
	/* 1. 显示页面 */
	ShowMainPage(g_atMainPageLayout);

	/* 2. 创建Prepare线程 */

	/* 3. 调用GetInputEvent获得输入事件，进而处理 */
	while (1)
	{
		InputEvent = MainPageGetInputEvent();
		switch (InputEvent)	
		{
			case "浏览模式":
			{
				/* 保存当前页面 */
				StorePage();
				
				Page("explore")->Run();
				
				/* 恢复之前的页面 */
				RestorePage();
				break;
			}
			case "连播模式":
			{
				/* 保存当前页面 */
				StorePage();

				Page("auto")->Run();
				break;
			}
			case "设置":
			{
				/* 保存当前页面 */
				StorePage();
				Page("setting")->Run();
				/* 恢复之前的页面 */
				RestorePage();
				break;
			}		
		}
	}
}


int MainPageGetInputEvent(...)
{
	/* 获得原始的触摸屏数据 
	 * 它是调用input_manager.c的函数，此函数会让当前线否休眠
	 * 当触摸屏线程获得数据后，会把它唤醒
	 */
	GetInputEvent();
	
	/* 处理数据 */
}

int MainPageInit(void)
{
	return RegisterPageAction(&g_tMainPageAction);
}

