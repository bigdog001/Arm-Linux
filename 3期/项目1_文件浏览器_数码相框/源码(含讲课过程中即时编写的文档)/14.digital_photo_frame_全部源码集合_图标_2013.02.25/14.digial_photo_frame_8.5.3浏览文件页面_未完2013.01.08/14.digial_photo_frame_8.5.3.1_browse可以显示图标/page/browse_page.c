#include <config.h>
#include <render.h>
#include <stdlib.h>

static T_Layout g_atBrowsePageIconsLayout[] = {
	{0, 0, 0, 0, "return.bmp"},
	{0, 0, 0, 0, "up.bmp"},
	{0, 0, 0, 0, "select.bmp"},
	{0, 0, 0, 0, "pre_page.bmp"},
	{0, 0, 0, 0, "next_page.bmp"},
	{0, 0, 0, 0, NULL},
};

static T_PageLayout g_tBrowsePageLayout = {
	.iMaxTotalBytes = 0,
	.atLayout       = g_atBrowsePageIconsLayout,
};


/* 计算各图标坐标值 */
static void  CalcBrowsePageLayout(PT_PageLayout ptPageLayout)
{
	int iWidth;
	int iHeight;
	int iXres, iYres, iBpp;
	int iTmpTotalBytes;
	PT_Layout atLayout;

	atLayout = ptPageLayout->atLayout;
	GetDispResolution(&iXres, &iYres, &iBpp);
	ptPageLayout->iBpp = iBpp;

	/*   iXres/4
	 *    ----------------------------------
	 *     up   select  pre_page  next_page
	 *
	 *
	 *
	 *
	 *
	 *
	 *    ----------------------------------
	 */
	 
	iWidth  = iXres / 5;
	iHeight = iWidth;
	
	/* up图标 */
	atLayout[0].iTopLeftY  = 0;
	atLayout[0].iBotRightY = atLayout[0].iTopLeftY + iHeight - 1;
	atLayout[0].iTopLeftX  = 0;
	atLayout[0].iBotRightX = atLayout[0].iTopLeftX + iWidth - 1;

	iTmpTotalBytes = (atLayout[0].iBotRightX - atLayout[0].iTopLeftX + 1) * (atLayout[0].iBotRightY - atLayout[0].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}

	/* select图标 */
	atLayout[1].iTopLeftY  = 0;
	atLayout[1].iBotRightY = atLayout[1].iTopLeftY + iHeight - 1;
	atLayout[1].iTopLeftX  = atLayout[0].iBotRightX + 1;
	atLayout[1].iBotRightX = atLayout[1].iTopLeftX + iWidth - 1;

	iTmpTotalBytes = (atLayout[1].iBotRightX - atLayout[1].iTopLeftX + 1) * (atLayout[1].iBotRightY - atLayout[1].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}

	/* pre_page图标 */
	atLayout[2].iTopLeftY  = 0;
	atLayout[2].iBotRightY = atLayout[2].iTopLeftY + iHeight - 1;
	atLayout[2].iTopLeftX  = atLayout[1].iBotRightX + 1;
	atLayout[2].iBotRightX = atLayout[2].iTopLeftX + iWidth - 1;

	iTmpTotalBytes = (atLayout[2].iBotRightX - atLayout[2].iTopLeftX + 1) * (atLayout[2].iBotRightY - atLayout[2].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}

	/* next_page图标 */
	atLayout[3].iTopLeftY  = 0;
	atLayout[3].iBotRightY = atLayout[3].iTopLeftY + iHeight - 1;
	atLayout[3].iTopLeftX  = atLayout[2].iBotRightX + 1;
	atLayout[3].iBotRightX = atLayout[3].iTopLeftX + iWidth - 1;

	iTmpTotalBytes = (atLayout[3].iBotRightX - atLayout[3].iTopLeftX + 1) * (atLayout[3].iBotRightY - atLayout[3].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}

	atLayout[4].iTopLeftY  = 0;
	atLayout[4].iBotRightY = atLayout[4].iTopLeftY + iHeight - 1;
	atLayout[4].iTopLeftX  = atLayout[3].iBotRightX + 1;
	atLayout[4].iBotRightX = atLayout[4].iTopLeftX + iWidth - 1;

	iTmpTotalBytes = (atLayout[4].iBotRightX - atLayout[4].iTopLeftX + 1) * (atLayout[4].iBotRightY - atLayout[4].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}

}

static int BrowsePageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	return GenericGetInputEvent(ptPageLayout, ptInputEvent);
}

static void ShowBrowsePage(PT_PageLayout ptPageLayout)
{
	PT_VideoMem ptVideoMem;
	int iError;

	PT_Layout atLayout = ptPageLayout->atLayout;
		
	/* 1. 获得显存 */
	ptVideoMem = GetVideoMem(ID("main"), 1);
	if (ptVideoMem == NULL)
	{
		DBG_PRINTF("can't get video mem for main page!\n");
		return;
	}

	/* 2. 描画数据 */

	/* 如果还没有计算过各图标的坐标 */
	if (atLayout[0].iTopLeftX == 0)
	{
		CalcBrowsePageLayout(ptPageLayout);
	}

	iError = GeneratePage(ptPageLayout, ptVideoMem);	

	/* 3. 刷到设备上去 */
	FlushVideoMemToDev(ptVideoMem);

	/* 4. 解放显存 */
	PutVideoMem(ptVideoMem);
}

static void BrowsePageRun(void)
{
	int iIndex;
	T_InputEvent tInputEvent;
	int bPressed = 0;
	int iIndexPressed = -1;
	
	/* 1. 显示页面 */
	ShowBrowsePage(&g_tBrowsePageLayout);

	/* 2. 创建Prepare线程 */

	/* 3. 调用GetInputEvent获得输入事件，进而处理 */
	while (1)
	{
		iIndex = BrowsePageGetInputEvent(&g_tBrowsePageLayout, &tInputEvent);
		if (tInputEvent.iPressure == 0)
		{
			/* 如果是松开 */
			if (bPressed)
			{
				/* 曾经有按钮被按下 */
				ReleaseButton(&g_atBrowsePageIconsLayout[iIndexPressed]);
				bPressed = 0;

				if (iIndexPressed == iIndex) /* 按下和松开都是同一个按钮 */
				{
					switch (iIndexPressed)
					{
						case 0: /* "向上"按钮 */
						{
							return;
							break;
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
					PressButton(&g_atBrowsePageIconsLayout[iIndexPressed]);
				}
			}
		}		
	}
}

static T_PageAction g_tBrowsePageAction = {
	.name          = "browse",
	.Run           = BrowsePageRun,
	.GetInputEvent = BrowsePageGetInputEvent,
	//.Prepare       = BrowsePagePrepare;
};

int BrowsePageInit(void)
{
	return RegisterPageAction(&g_tBrowsePageAction);
}

