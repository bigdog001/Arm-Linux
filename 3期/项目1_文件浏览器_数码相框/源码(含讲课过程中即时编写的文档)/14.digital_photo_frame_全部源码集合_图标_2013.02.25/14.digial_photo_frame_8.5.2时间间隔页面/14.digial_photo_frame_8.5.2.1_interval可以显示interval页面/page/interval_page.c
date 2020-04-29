#include <config.h>
#include <render.h>
#include <stdlib.h>


static T_Layout g_atIntervalPageIconsLayout[] = {
	{0, 0, 0, 0, "inc.bmp"},
	{0, 0, 0, 0, "time.bmp"},
	{0, 0, 0, 0, "dec.bmp"},
	{0, 0, 0, 0, "ok.bmp"},
	{0, 0, 0, 0, "cancel.bmp"},
	{0, 0, 0, 0, NULL},
};

static T_PageLayout g_tIntervalPageLayout = {
	.iMaxTotalBytes = 0,
	.atLayout       = g_atIntervalPageIconsLayout,
};


/* �����ͼ������ֵ */
static void  IntervalPageCalcLayout(PT_PageLayout ptPageLayout)
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

	/* incͼ�� */
	atLayout[0].iTopLeftY  = iStartY;
	atLayout[0].iBotRightY = atLayout[0].iTopLeftY + iHeight * 28 / 128 - 1;
	atLayout[0].iTopLeftX  = (iXres - iWidth * 52 / 128) / 2;
	atLayout[0].iBotRightX = atLayout[0].iTopLeftX + iWidth * 52 / 128 - 1;

	iTmpTotalBytes = (atLayout[0].iBotRightX - atLayout[0].iTopLeftX + 1) * (atLayout[0].iBotRightY - atLayout[0].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}

	/* timeͼ�� */
	atLayout[1].iTopLeftY  = atLayout[0].iBotRightY + 1;
	atLayout[1].iBotRightY = atLayout[1].iTopLeftY + iHeight * 72 / 128 - 1;
	atLayout[1].iTopLeftX  = (iXres - iWidth) / 2;
	atLayout[1].iBotRightX = atLayout[1].iTopLeftX + iWidth - 1;
	iTmpTotalBytes = (atLayout[1].iBotRightX - atLayout[1].iTopLeftX + 1) * (atLayout[1].iBotRightY - atLayout[1].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}

	/* decͼ�� */
	atLayout[2].iTopLeftY  = atLayout[1].iBotRightY + 1;
	atLayout[2].iBotRightY = atLayout[2].iTopLeftY + iHeight * 28 / 128 - 1;
	atLayout[2].iTopLeftX  = (iXres - iWidth * 52 / 128) / 2;
	atLayout[2].iBotRightX = atLayout[2].iTopLeftX + iWidth * 52 / 128 - 1;
	iTmpTotalBytes = (atLayout[2].iBotRightX - atLayout[2].iTopLeftX + 1) * (atLayout[2].iBotRightY - atLayout[2].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}

	/* okͼ�� */
	atLayout[3].iTopLeftY  = atLayout[2].iBotRightY + iHeight / 2 + 1;
	atLayout[3].iBotRightY = atLayout[3].iTopLeftY + iHeight / 2 - 1;
	atLayout[3].iTopLeftX  = (iXres - iWidth) / 3;
	atLayout[3].iBotRightX = atLayout[3].iTopLeftX + iWidth / 2 - 1;
	iTmpTotalBytes = (atLayout[3].iBotRightX - atLayout[3].iTopLeftX + 1) * (atLayout[3].iBotRightY - atLayout[3].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}

	/* okͼ�� */
	atLayout[4].iTopLeftY  = atLayout[3].iTopLeftY;
	atLayout[4].iBotRightY = atLayout[3].iBotRightY;
	atLayout[4].iTopLeftX  = atLayout[3].iTopLeftX * 2 + iWidth/2;
	atLayout[4].iBotRightX = atLayout[4].iTopLeftX + iWidth/2 - 1;
	iTmpTotalBytes = (atLayout[4].iBotRightX - atLayout[4].iTopLeftX + 1) * (atLayout[4].iBotRightY - atLayout[4].iTopLeftY + 1) * iBpp / 8;
	if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
	{
		ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
	}
			
}


static int IntervalPageGetInputEvent2(PT_Layout atLayout, PT_InputEvent ptInputEvent)
{
	T_InputEvent tInputEvent;
	int iRet;
	int i = 0;
	//PT_Layout atLayout = ptPageLayout->atLayout;
	
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

#if 0
static int IntervalPageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	T_InputEvent tInputEvent;
	int iRet;
	int i = 0;
	PT_Layout atLayout = ptPageLayout->atLayout;
	
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
#endif
static void ShowIntervalPage(PT_PageLayout ptPageLayout)
{
	PT_VideoMem ptVideoMem;
	int iError;
	PT_Layout atLayout = ptPageLayout->atLayout;
		
	/* 1. ����Դ� */
	ptVideoMem = GetVideoMem(ID("interval"), 1);
	if (ptVideoMem == NULL)
	{
		DBG_PRINTF("can't get video mem for interval page!\n");
		return;
	}

	/* 2. �軭���� */
	/* �����û�м������ͼ������� */
	if (atLayout[0].iTopLeftX == 0)
	{
		IntervalPageCalcLayout(ptPageLayout);
	}

	iError = GeneratePage(ptPageLayout, ptVideoMem);
	
	/* 3. ˢ���豸��ȥ */
	FlushVideoMemToDev(ptVideoMem);

	/* 4. ����Դ� */
	PutVideoMem(ptVideoMem);
}

static void IntervalPageRun(void)
{
	int iIndex;
	T_InputEvent tInputEvent;
	int bPressed = 0;
	int iIndexPressed = -1;
	
	/* 1. ��ʾҳ�� */
	ShowIntervalPage(&g_tIntervalPageLayout);

	/* 2. ����Prepare�߳� */

	/* 3. ����GetInputEvent��������¼����������� */
	tInputEvent.iPressure = 0; /* ������뾯�� */
	while (1)
	{
		//iIndex = IntervalPageGetInputEvent(&g_tIntervalPageLayout, &tInputEvent);
		iIndex = IntervalPageGetInputEvent2(g_tIntervalPageLayout.atLayout, &tInputEvent);
		if (tInputEvent.iPressure == 0)
		{
			/* ������ɿ� */
			if (bPressed)
			{
				/* �����а�ť������ */
				ReleaseButton(&g_atIntervalPageIconsLayout[iIndexPressed]);
				bPressed = 0;

				if (iIndexPressed == iIndex) /* ���º��ɿ�����ͬһ����ť */
				{
					switch (iIndexPressed)
					{
						case 0: /* inc��ť */
						{
							break;
						}
						case 2: /* dec��ť */
						{
							break;
						}
						case 3: /* ok��ť */
						{
							return;
							break;
						}
						case 4: /* cancel��ť */
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
				if (!bPressed && (iIndex != 1))
				{
					/* δ�����°�ť */
					bPressed = 1;
					iIndexPressed = iIndex;
					PressButton(&g_atIntervalPageIconsLayout[iIndexPressed]);
				}
			}
		}		
	}
}

static T_PageAction g_tIntervalPageAction = {
	.name          = "interval",
	.Run           = IntervalPageRun,
	//.GetInputEvent = IntervalPageGetInputEvent,
	//.Prepare       = MainPagePrepare;
};

int IntervalPageInit(void)
{
	return RegisterPageAction(&g_tIntervalPageAction);
}



