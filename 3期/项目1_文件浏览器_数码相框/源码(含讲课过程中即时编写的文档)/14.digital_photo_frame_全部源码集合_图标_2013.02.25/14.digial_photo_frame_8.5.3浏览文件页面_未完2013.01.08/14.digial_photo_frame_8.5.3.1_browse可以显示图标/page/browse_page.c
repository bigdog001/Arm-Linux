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


/* �����ͼ������ֵ */
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
	
	/* upͼ�� */
	atLayout[0].iTopLeftY  = 0;
	atLayout[0].iBotRightY = atLayout[0].iTopLeftY + iHeight - 1;
	atLayout[0].iTopLeftX  = 0;
	atLayout[0].iBotRightX = atLayout[0].iTopLeftX + iWidth - 1;

	iTmpTotalBytes = (atLayout[0].iBotRightX - atLayout[0].iTopLeftX + 1) * (atLayout[0].iBotRightY - atLayout[0].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}

	/* selectͼ�� */
	atLayout[1].iTopLeftY  = 0;
	atLayout[1].iBotRightY = atLayout[1].iTopLeftY + iHeight - 1;
	atLayout[1].iTopLeftX  = atLayout[0].iBotRightX + 1;
	atLayout[1].iBotRightX = atLayout[1].iTopLeftX + iWidth - 1;

	iTmpTotalBytes = (atLayout[1].iBotRightX - atLayout[1].iTopLeftX + 1) * (atLayout[1].iBotRightY - atLayout[1].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}

	/* pre_pageͼ�� */
	atLayout[2].iTopLeftY  = 0;
	atLayout[2].iBotRightY = atLayout[2].iTopLeftY + iHeight - 1;
	atLayout[2].iTopLeftX  = atLayout[1].iBotRightX + 1;
	atLayout[2].iBotRightX = atLayout[2].iTopLeftX + iWidth - 1;

	iTmpTotalBytes = (atLayout[2].iBotRightX - atLayout[2].iTopLeftX + 1) * (atLayout[2].iBotRightY - atLayout[2].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}

	/* next_pageͼ�� */
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
		
	/* 1. ����Դ� */
	ptVideoMem = GetVideoMem(ID("main"), 1);
	if (ptVideoMem == NULL)
	{
		DBG_PRINTF("can't get video mem for main page!\n");
		return;
	}

	/* 2. �軭���� */

	/* �����û�м������ͼ������� */
	if (atLayout[0].iTopLeftX == 0)
	{
		CalcBrowsePageLayout(ptPageLayout);
	}

	iError = GeneratePage(ptPageLayout, ptVideoMem);	

	/* 3. ˢ���豸��ȥ */
	FlushVideoMemToDev(ptVideoMem);

	/* 4. ����Դ� */
	PutVideoMem(ptVideoMem);
}

static void BrowsePageRun(void)
{
	int iIndex;
	T_InputEvent tInputEvent;
	int bPressed = 0;
	int iIndexPressed = -1;
	
	/* 1. ��ʾҳ�� */
	ShowBrowsePage(&g_tBrowsePageLayout);

	/* 2. ����Prepare�߳� */

	/* 3. ����GetInputEvent��������¼����������� */
	while (1)
	{
		iIndex = BrowsePageGetInputEvent(&g_tBrowsePageLayout, &tInputEvent);
		if (tInputEvent.iPressure == 0)
		{
			/* ������ɿ� */
			if (bPressed)
			{
				/* �����а�ť������ */
				ReleaseButton(&g_atBrowsePageIconsLayout[iIndexPressed]);
				bPressed = 0;

				if (iIndexPressed == iIndex) /* ���º��ɿ�����ͬһ����ť */
				{
					switch (iIndexPressed)
					{
						case 0: /* "����"��ť */
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
			/* ����״̬ */
			if (iIndex != -1)
			{
				if (!bPressed)
				{
					/* δ�����°�ť */
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

