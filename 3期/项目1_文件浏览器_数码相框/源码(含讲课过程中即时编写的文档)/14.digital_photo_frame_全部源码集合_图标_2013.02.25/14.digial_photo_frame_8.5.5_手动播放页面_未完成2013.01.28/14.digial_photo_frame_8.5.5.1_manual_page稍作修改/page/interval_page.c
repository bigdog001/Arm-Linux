#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <fonts_manager.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>


static T_Layout g_atIntervalPageIconsLayout[] = {
	{0, 0, 0, 0, "inc.bmp"},
	{0, 0, 0, 0, "time.bmp"},
	{0, 0, 0, 0, "dec.bmp"},
	{0, 0, 0, 0, "ok.bmp"},
	{0, 0, 0, 0, "cancel.bmp"},
	{0, 0, 0, 0, NULL},
};

/* 显示数字的地方 */
static T_Layout g_tIntervalNumberLayout;

static int g_iIntervalSecond = 1;

static T_PageLayout g_tIntervalPageLayout = {
	.iMaxTotalBytes = 0,
	.atLayout       = g_atIntervalPageIconsLayout,
};

void GetIntervalTime(int *piIntervalSecond)
{
    *piIntervalSecond = g_iIntervalSecond;
}


/* 计算各图标坐标值 */
static void  CalcIntervalPageLayout(PT_PageLayout ptPageLayout)
{
	int iStartY;
	int iWidth;
	int iHeight;
	int iXres, iYres, iBpp;
	int iTmpTotalBytes;
	PT_Layout atLayout;

	atLayout = ptPageLayout->atLayout;
	GetDispResolution(&iXres, &iYres, &iBpp);
	ptPageLayout->iBpp = iBpp;

	/*   
	 *    ----------------------
	 *                          1/2 * iHeight
	 *          inc.bmp         iHeight * 28 / 128     
	 *         time.bmp         iHeight * 72 / 128
	 *          dec.bmp         iHeight * 28 / 128     
	 *                          1/2 * iHeight
	 *    ok.bmp     cancel.bmp 1/2 * iHeight
	 *                          1/2 * iHeight
	 *    ----------------------
	 */
	iHeight = iYres / 3;
	iWidth  = iHeight;
	iStartY = iHeight / 2;

	/* inc图标 */
	atLayout[0].iTopLeftY  = iStartY;
	atLayout[0].iBotRightY = atLayout[0].iTopLeftY + iHeight * 28 / 128 - 1;
	atLayout[0].iTopLeftX  = (iXres - iWidth * 52 / 128) / 2;
	atLayout[0].iBotRightX = atLayout[0].iTopLeftX + iWidth * 52 / 128 - 1;

	iTmpTotalBytes = (atLayout[0].iBotRightX - atLayout[0].iTopLeftX + 1) * (atLayout[0].iBotRightY - atLayout[0].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}

	/* time图标 */
	atLayout[1].iTopLeftY  = atLayout[0].iBotRightY + 1;
	atLayout[1].iBotRightY = atLayout[1].iTopLeftY + iHeight * 72 / 128 - 1;
	atLayout[1].iTopLeftX  = (iXres - iWidth) / 2;
	atLayout[1].iBotRightX = atLayout[1].iTopLeftX + iWidth - 1;
	iTmpTotalBytes = (atLayout[1].iBotRightX - atLayout[1].iTopLeftX + 1) * (atLayout[1].iBotRightY - atLayout[1].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}

	/* dec图标 */
	atLayout[2].iTopLeftY  = atLayout[1].iBotRightY + 1;
	atLayout[2].iBotRightY = atLayout[2].iTopLeftY + iHeight * 28 / 128 - 1;
	atLayout[2].iTopLeftX  = (iXres - iWidth * 52 / 128) / 2;
	atLayout[2].iBotRightX = atLayout[2].iTopLeftX + iWidth * 52 / 128 - 1;
	iTmpTotalBytes = (atLayout[2].iBotRightX - atLayout[2].iTopLeftX + 1) * (atLayout[2].iBotRightY - atLayout[2].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}

	/* ok图标 */
	atLayout[3].iTopLeftY  = atLayout[2].iBotRightY + iHeight / 2 + 1;
	atLayout[3].iBotRightY = atLayout[3].iTopLeftY + iHeight / 2 - 1;
	atLayout[3].iTopLeftX  = (iXres - iWidth) / 3;
	atLayout[3].iBotRightX = atLayout[3].iTopLeftX + iWidth / 2 - 1;
	iTmpTotalBytes = (atLayout[3].iBotRightX - atLayout[3].iTopLeftX + 1) * (atLayout[3].iBotRightY - atLayout[3].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}

	/* ok图标 */
	atLayout[4].iTopLeftY  = atLayout[3].iTopLeftY;
	atLayout[4].iBotRightY = atLayout[3].iBotRightY;
	atLayout[4].iTopLeftX  = atLayout[3].iTopLeftX * 2 + iWidth/2;
	atLayout[4].iBotRightX = atLayout[4].iTopLeftX + iWidth/2 - 1;
	iTmpTotalBytes = (atLayout[4].iBotRightX - atLayout[4].iTopLeftX + 1) * (atLayout[4].iBotRightY - atLayout[4].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}

	/* 用来显示数字的区域比较特殊, 单独处理
	 * time.bmp原图大小为128x72, 里面的两个数字大小为52x40
	 * 经过CalcIntervalPageLayout后有所缩放
	 */
	iWidth  = atLayout[1].iBotRightX - atLayout[1].iTopLeftX + 1;
	iHeight = atLayout[1].iBotRightY - atLayout[1].iTopLeftY + 1;

	g_tIntervalNumberLayout.iTopLeftX  = atLayout[1].iTopLeftX + (128 - 52) / 2 * iWidth / 128;
	g_tIntervalNumberLayout.iBotRightX = atLayout[1].iBotRightX - (128 - 52) / 2 * iWidth / 128 + 1;

	g_tIntervalNumberLayout.iTopLeftY  = atLayout[1].iTopLeftY + (72 - 40) / 2 * iHeight / 72;
	g_tIntervalNumberLayout.iBotRightY = atLayout[1].iBotRightY - (72 - 40) / 2 * iHeight / 72 + 1;

}
 
static int IntervalPageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	return GenericGetInputEvent(ptPageLayout, ptInputEvent);
}

/* 特殊图标: 绘制图标中的数字 */
static int GenerateIntervalPageSpecialIcon(int dwNumber, PT_VideoMem ptVideoMem)
{
	unsigned int dwFontSize;
	char strNumber[3];
	int iError;
	

	dwFontSize = g_tIntervalNumberLayout.iBotRightY - g_tIntervalNumberLayout.iTopLeftY;
	SetFontSize(dwFontSize);

	/* 显示两位数字: 00~59 */
	if (dwNumber > 59)
	{
		return -1;
	}

	snprintf(strNumber, 3, "%02d", dwNumber);
	//DBG_PRINTF("strNumber = %s, len = %d\n", strNumber, strlen(strNumber));

	iError = MergerStringToCenterOfRectangleInVideoMem(g_tIntervalNumberLayout.iTopLeftX, g_tIntervalNumberLayout.iTopLeftY, g_tIntervalNumberLayout.iBotRightX, g_tIntervalNumberLayout.iBotRightY, (unsigned char *)strNumber, ptVideoMem);

	return iError;
}

static void ShowIntervalPage(PT_PageLayout ptPageLayout)
{
	PT_VideoMem ptVideoMem;
	int iError;
	PT_Layout atLayout = ptPageLayout->atLayout;
		
	/* 1. 获得显存 */
	ptVideoMem = GetVideoMem(ID("interval"), 1);
	if (ptVideoMem == NULL)
	{
		DBG_PRINTF("can't get video mem for interval page!\n");
		return;
	}

	/* 2. 描画数据 */
	/* 如果还没有计算过各图标的坐标 */
	if (atLayout[0].iTopLeftX == 0)
	{
		CalcIntervalPageLayout(ptPageLayout);
	}

	iError = GeneratePage(ptPageLayout, ptVideoMem);

	iError = GenerateIntervalPageSpecialIcon(g_iIntervalSecond, ptVideoMem);
	if (iError)
	{
		DBG_PRINTF("GenerateIntervalPageSpecialIcon error!\n");
	}
	
	/* 3. 刷到设备上去 */
	FlushVideoMemToDev(ptVideoMem);

	/* 4. 解放显存 */
	PutVideoMem(ptVideoMem);
}

static void IntervalPageRun(void *pVoid)
{
	int iIndex;
	T_InputEvent tInputEvent;
	T_InputEvent tInputEventPrePress;
	int bPressed = 0;
	int iIndexPressed = -1;
	int iIntervalSecond = g_iIntervalSecond;
	PT_VideoMem ptDevVideoMem;
	int bFast = 0;  /* 表示快速加减 */

	ptDevVideoMem = GetDevVideoMem();

	/* 这两句只是为了避免编译警告 */
	tInputEventPrePress.tTime.tv_sec = 0;
	tInputEventPrePress.tTime.tv_usec = 0;
	
	/* 1. 显示页面 */
	ShowIntervalPage(&g_tIntervalPageLayout);

	/* 2. 创建Prepare线程 */

	/* 3. 调用GetInputEvent获得输入事件，进而处理 */
	tInputEvent.iPressure = 0; /* 避免编译警告 */
	while (1)
	{
		iIndex = IntervalPageGetInputEvent(&g_tIntervalPageLayout, &tInputEvent);
 		if (tInputEvent.iPressure == 0)
		{
			/* 如果是松开 */
			if (bPressed)
			{
				bFast = 0;
				
				/* 曾经有按钮被按下 */
				ReleaseButton(&g_atIntervalPageIconsLayout[iIndexPressed]);
				bPressed = 0;

				if (iIndexPressed == iIndex) /* 按下和松开都是同一个按钮 */
				{
					switch (iIndexPressed)
					{
						case 0: /* inc按钮 */
						{
							iIntervalSecond++;
							if (iIntervalSecond == 60)
							{
								iIntervalSecond = 0;
							}
							GenerateIntervalPageSpecialIcon(iIntervalSecond, ptDevVideoMem);
							break;
						}
						case 2: /* dec按钮 */
						{
							iIntervalSecond--;
							if (iIntervalSecond == -1)
							{
								iIntervalSecond = 59;
							}
							GenerateIntervalPageSpecialIcon(iIntervalSecond, ptDevVideoMem);
							break;
						}
						case 3: /* ok按钮 */
						{
							g_iIntervalSecond = iIntervalSecond;
							
							return;
							break;
						}
						case 4: /* cancel按钮 */
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
				if (!bPressed && (iIndex != 1))
				{
					/* 未曾按下按钮 */
					bPressed = 1;
					iIndexPressed = iIndex;
					tInputEventPrePress = tInputEvent;  /* 记录下来 */
					PressButton(&g_atIntervalPageIconsLayout[iIndexPressed]);
				}

				/* 如果按下的是"inc.bmp"或"dec.bmp" 
				 * 连按2秒后, 飞快的递增或减小: 每50ms变化一次
				 */
				if ((iIndexPressed == 0) || (iIndexPressed == 2))
				{
					if (bFast && (TimeMSBetween(tInputEventPrePress.tTime, tInputEvent.tTime) > 50))
					{
						iIntervalSecond = iIndexPressed ? (iIntervalSecond - 1) : (iIntervalSecond + 1);
						if (iIntervalSecond == 60)
						{
							iIntervalSecond = 0;
						}
						else if (iIntervalSecond == -1)
						{
							iIntervalSecond = 59;
						}
						GenerateIntervalPageSpecialIcon(iIntervalSecond, ptDevVideoMem);
						tInputEventPrePress = tInputEvent;
					}
					
					if (TimeMSBetween(tInputEventPrePress.tTime, tInputEvent.tTime) > 2000)
					{
						bFast = 1;
						tInputEventPrePress = tInputEvent;
					}
					
				}
			}
		}		
	}
}

static T_PageAction g_tIntervalPageAction = {
	.name          = "interval",
	.Run           = IntervalPageRun,
	.GetInputEvent = IntervalPageGetInputEvent,
	//.Prepare       = MainPagePrepare;
};

int IntervalPageInit(void)
{
	return RegisterPageAction(&g_tIntervalPageAction);
}



