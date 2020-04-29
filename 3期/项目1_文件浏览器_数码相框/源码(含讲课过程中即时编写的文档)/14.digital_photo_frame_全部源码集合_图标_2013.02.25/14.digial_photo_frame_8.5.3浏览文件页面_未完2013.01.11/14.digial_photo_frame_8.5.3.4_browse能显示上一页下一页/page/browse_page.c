#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <file.h>
#include <fonts_manager.h>


/* ͼ����һ��������, "ͼ��+����"Ҳ��һ��������
 *   --------
 *   |  ͼ  |
 *   |  ��  |
 * ------------
 * |   ����   |
 * ------------
 */

#define DIR_FILE_ICON_WIDTH    40
#define DIR_FILE_ICON_HEIGHT   DIR_FILE_ICON_WIDTH
#define DIR_FILE_NAME_HEIGHT   20
#define DIR_FILE_NAME_WIDTH   (DIR_FILE_ICON_HEIGHT + DIR_FILE_NAME_HEIGHT)
#define DIR_FILE_ALL_WIDTH    DIR_FILE_NAME_WIDTH
#define DIR_FILE_ALL_HEIGHT   DIR_FILE_ALL_WIDTH


/* browseҳ�������ʾ�����Ϊ"�˵�"��"Ŀ¼���ļ�"
 * "�˵�"����"up, select,pre_page,next_page"�ĸ��ɲ�����ͼ��
 * "Ŀ¼���ļ�"�����������
 */

/* �˵������� */
static T_Layout g_atBrowsePageMenusLayout[] = {
//	{0, 0, 0, 0, "return.bmp"},
	{0, 0, 0, 0, "up.bmp"},
	{0, 0, 0, 0, "select.bmp"},
	{0, 0, 0, 0, "pre_page.bmp"},
	{0, 0, 0, 0, "next_page.bmp"},
	{0, 0, 0, 0, NULL},
};

static T_PageLayout g_tBrowsePageLayout = {
	.iMaxTotalBytes = 0,
	.atLayout       = g_atBrowsePageMenusLayout,
};

/* Ŀ¼���ļ������� */
static char *g_strDirIconName  = "fold_closed.bmp";
static char *g_strFileIconName = "file.bmp";
static T_Layout *g_atDirAndFileLayout;
static T_PageLayout g_tBrowsePageDirAndFileLayout = {
	.iMaxTotalBytes = 0,
	//.atLayout       = g_atDirAndFileLayout,
};

static int g_iDirFileNumPerCol, g_iDirFileNumPerRow;

/* ��������ĳĿ¼������� */
static PT_DirContent *g_aptDirContents;  /* ����:����Ŀ¼��"������Ŀ¼","�ļ�"������ */
static int g_iDirContentsNumber;         /* g_aptDirContents�����ж����� */
static int g_iStartIndex = 0;            /* ����Ļ����ʾ�ĵ�1��"Ŀ¼���ļ�"��g_aptDirContents���������һ�� */

/* ��ǰ��ʾ��Ŀ¼ */
static char g_strCurDir[256] = DEFAULT_DIR;

/* ����˵��и�ͼ������ֵ */
static void  CalcBrowsePageMenusLayout(PT_PageLayout ptPageLayout)
{
	int iWidth;
	int iHeight;
	int iXres, iYres, iBpp;
	int iTmpTotalBytes;
	PT_Layout atLayout;
	int i;

	atLayout = ptPageLayout->atLayout;
	GetDispResolution(&iXres, &iYres, &iBpp);
	ptPageLayout->iBpp = iBpp;

	if (iXres < iYres)
	{			 
		/*	 iXres/4
		 *	  ----------------------------------
		 *	   up	select	pre_page  next_page
		 *
		 *
		 *
		 *
		 *
		 *
		 *	  ----------------------------------
		 */
		 
		iWidth  = iXres / 4;
		iHeight = iWidth;
		
		/* returnͼ�� */
		atLayout[0].iTopLeftY  = 0;
		atLayout[0].iBotRightY = atLayout[0].iTopLeftY + iHeight - 1;
		atLayout[0].iTopLeftX  = 0;
		atLayout[0].iBotRightX = atLayout[0].iTopLeftX + iWidth - 1;

		/* upͼ�� */
		atLayout[1].iTopLeftY  = 0;
		atLayout[1].iBotRightY = atLayout[1].iTopLeftY + iHeight - 1;
		atLayout[1].iTopLeftX  = atLayout[0].iBotRightX + 1;
		atLayout[1].iBotRightX = atLayout[1].iTopLeftX + iWidth - 1;

		/* selectͼ�� */
		atLayout[2].iTopLeftY  = 0;
		atLayout[2].iBotRightY = atLayout[2].iTopLeftY + iHeight - 1;
		atLayout[2].iTopLeftX  = atLayout[1].iBotRightX + 1;
		atLayout[2].iBotRightX = atLayout[2].iTopLeftX + iWidth - 1;

		/* pre_pageͼ�� */
		atLayout[3].iTopLeftY  = 0;
		atLayout[3].iBotRightY = atLayout[3].iTopLeftY + iHeight - 1;
		atLayout[3].iTopLeftX  = atLayout[2].iBotRightX + 1;
		atLayout[3].iBotRightX = atLayout[3].iTopLeftX + iWidth - 1;
#if 0
		/* next_pageͼ�� */
		atLayout[4].iTopLeftY  = 0;
		atLayout[4].iBotRightY = atLayout[4].iTopLeftY + iHeight - 1;
		atLayout[4].iTopLeftX  = atLayout[3].iBotRightX + 1;
		atLayout[4].iBotRightX = atLayout[4].iTopLeftX + iWidth - 1;
#endif
	}
	else
	{
		/*	 iYres/4
		 *	  ----------------------------------
		 *	   up		  
		 *
		 *    select
		 *
		 *    pre_page
		 *  
		 *   next_page
		 *
		 *	  ----------------------------------
		 */
		 
		iHeight  = iYres / 4;
		iWidth = iHeight;

		/* returnͼ�� */
		atLayout[0].iTopLeftY  = 0;
		atLayout[0].iBotRightY = atLayout[0].iTopLeftY + iHeight - 1;
		atLayout[0].iTopLeftX  = 0;
		atLayout[0].iBotRightX = atLayout[0].iTopLeftX + iWidth - 1;
		
		/* upͼ�� */
		atLayout[1].iTopLeftY  = atLayout[0].iBotRightY+ 1;
		atLayout[1].iBotRightY = atLayout[1].iTopLeftY + iHeight - 1;
		atLayout[1].iTopLeftX  = 0;
		atLayout[1].iBotRightX = atLayout[1].iTopLeftX + iWidth - 1;
		
		/* selectͼ�� */
		atLayout[2].iTopLeftY  = atLayout[1].iBotRightY + 1;
		atLayout[2].iBotRightY = atLayout[2].iTopLeftY + iHeight - 1;
		atLayout[2].iTopLeftX  = 0;
		atLayout[2].iBotRightX = atLayout[2].iTopLeftX + iWidth - 1;
		
		/* pre_pageͼ�� */
		atLayout[3].iTopLeftY  = atLayout[2].iBotRightY + 1;
		atLayout[3].iBotRightY = atLayout[3].iTopLeftY + iHeight - 1;
		atLayout[3].iTopLeftX  = 0;
		atLayout[3].iBotRightX = atLayout[3].iTopLeftX + iWidth - 1;
#if 0		
		/* next_pageͼ�� */
		atLayout[4].iTopLeftY  = atLayout[3].iBotRightY + 1;
		atLayout[4].iBotRightY = atLayout[4].iTopLeftY + iHeight - 1;
		atLayout[4].iTopLeftX  = 0;
		atLayout[4].iBotRightX = atLayout[4].iTopLeftX + iWidth - 1;
#endif		
	}

	i = 0;
	while (atLayout[i].strIconName)
	{
		iTmpTotalBytes = (atLayout[i].iBotRightX - atLayout[i].iTopLeftX + 1) * (atLayout[i].iBotRightY - atLayout[i].iTopLeftY + 1) * iBpp / 8;
		if (ptPageLayout->iMaxTotalBytes < iTmpTotalBytes)
		{
			ptPageLayout->iMaxTotalBytes = iTmpTotalBytes;
		}
		i++;
	}
}


/* ����Ŀ¼���ļ�����ʾ���� */
static int CalcBrowsePageDirAndFilesLayout(void)
{
	int iXres, iYres, iBpp;
	int iTopLeftX, iTopLeftY;
	int iTopLeftXBak;
	int iBotRightX, iBotRightY;
    int iIconWidth, iIconHeight;
    int iNumPerCol, iNumPerRow;
    int iDeltaX, iDeltaY;
    int i, j, k = 0;
	
	GetDispResolution(&iXres, &iYres, &iBpp);

	if (iXres < iYres)
	{
		/* --------------------------------------
		 *    up select pre_page next_page ͼ��
		 * --------------------------------------
		 *
		 *           Ŀ¼���ļ�
		 *
		 *
		 * --------------------------------------
		 */
		iTopLeftX  = 0;
		iBotRightX = iXres - 1;
		iTopLeftY  = g_atBrowsePageMenusLayout[0].iBotRightY + 1;
		iBotRightY = iYres - 1;
	}
	else
	{
		/*	 iYres/4
		 *	  ----------------------------------
		 *	   up      |
		 *             |
		 *    select   |
		 *             |     Ŀ¼���ļ�
		 *    pre_page |
		 *             |
		 *   next_page |
		 *             |
		 *	  ----------------------------------
		 */
		iTopLeftX  = g_atBrowsePageMenusLayout[0].iBotRightX + 1;
		iBotRightX = iXres - 1;
		iTopLeftY  = 0;
		iBotRightY = iYres - 1;
	}

    /* ȷ��һ����ʾ���ٸ�"Ŀ¼���ļ�", ��ʾ������ */
    iIconWidth  = DIR_FILE_NAME_WIDTH;
    iIconHeight = iIconWidth;

    /* ͼ��֮��ļ��Ҫ����10������ */
    iNumPerRow = (iBotRightX - iTopLeftX + 1) / iIconWidth;
    while (1)
    {
        iDeltaX  = (iBotRightX - iTopLeftX + 1) - iIconWidth * iNumPerRow;
        if ((iDeltaX / (iNumPerRow + 1)) < 10)
            iNumPerRow--;
        else
            break;
    }

    iNumPerCol = (iBotRightY - iTopLeftY + 1) / iIconHeight;
    while (1)
    {
        iDeltaY  = (iBotRightY - iTopLeftY + 1) - iIconHeight * iNumPerCol;
        if ((iDeltaY / (iNumPerCol + 1)) < 10)
            iNumPerCol--;
        else
            break;
    }

    /* ÿ��ͼ��֮��ļ�� */
    iDeltaX = iDeltaX / (iNumPerRow + 1);
    iDeltaY = iDeltaY / (iNumPerCol + 1);

    g_iDirFileNumPerRow = iNumPerRow;
    g_iDirFileNumPerCol = iNumPerCol;


    /* ������ʾ iNumPerRow * iNumPerCol��"Ŀ¼���ļ�"
     * ����"����+1"��T_Layout�ṹ��: һ��������ʾͼ��,��һ��������ʾ����
     * ���һ��������NULL,�����жϽṹ�������ĩβ
     */
	g_atDirAndFileLayout = malloc(sizeof(T_Layout) * (2 * iNumPerRow * iNumPerCol + 1));
    if (NULL == g_atDirAndFileLayout)
    {
        DBG_PRINTF("malloc error!\n");
        return -1;
    }

    /* "Ŀ¼���ļ�"������������Ͻǡ����½����� */
    g_tBrowsePageDirAndFileLayout.iTopLeftX      = iTopLeftX;
    g_tBrowsePageDirAndFileLayout.iBotRightX     = iBotRightX;
    g_tBrowsePageDirAndFileLayout.iTopLeftY      = iTopLeftY;
    g_tBrowsePageDirAndFileLayout.iBotRightY     = iBotRightY;
    g_tBrowsePageDirAndFileLayout.iBpp           = iBpp;
    g_tBrowsePageDirAndFileLayout.atLayout       = g_atDirAndFileLayout;
    g_tBrowsePageDirAndFileLayout.iMaxTotalBytes = DIR_FILE_ALL_WIDTH * DIR_FILE_ALL_HEIGHT * iBpp / 8;

    
    /* ȷ��ͼ������ֵ�λ�� 
     *
     * ͼ����һ��������, "ͼ��+����"Ҳ��һ��������
     *   --------
     *   |  ͼ  |
     *   |  ��  |
     * ------------
     * |   ����   |
     * ------------
     */
    iTopLeftX += iDeltaX;
    iTopLeftY += iDeltaY;
    iTopLeftXBak = iTopLeftX;
    for (i = 0; i < iNumPerCol; i++)
    {        
        for (j = 0; j < iNumPerRow; j++)
        {
            /* ͼ�� */
            g_atDirAndFileLayout[k].iTopLeftX  = iTopLeftX + (DIR_FILE_NAME_WIDTH - DIR_FILE_ICON_WIDTH) / 2;
            g_atDirAndFileLayout[k].iBotRightX = g_atDirAndFileLayout[k].iTopLeftX + DIR_FILE_ICON_WIDTH - 1;
            g_atDirAndFileLayout[k].iTopLeftY  = iTopLeftY;
            g_atDirAndFileLayout[k].iBotRightY = iTopLeftY + DIR_FILE_ICON_HEIGHT - 1;

            /* ���� */
            g_atDirAndFileLayout[k+1].iTopLeftX  = iTopLeftX;
            g_atDirAndFileLayout[k+1].iBotRightX = iTopLeftX + DIR_FILE_NAME_WIDTH - 1;
            g_atDirAndFileLayout[k+1].iTopLeftY  = g_atDirAndFileLayout[k].iBotRightY + 1;
            g_atDirAndFileLayout[k+1].iBotRightY = g_atDirAndFileLayout[k+1].iTopLeftY + DIR_FILE_NAME_HEIGHT - 1;

            iTopLeftX += DIR_FILE_ALL_WIDTH + iDeltaX;
            k += 2;
        }
        iTopLeftX = iTopLeftXBak;
        iTopLeftY += DIR_FILE_ALL_HEIGHT + iDeltaY;
    }

    /* ��β */
    g_atDirAndFileLayout[k].iTopLeftX   = 0;
    g_atDirAndFileLayout[k].iBotRightX  = 0;
    g_atDirAndFileLayout[k].iTopLeftY   = 0;
    g_atDirAndFileLayout[k].iBotRightY  = 0;
    g_atDirAndFileLayout[k].strIconName = NULL;

    return 0;
}

static int BrowsePageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	return GenericGetInputEvent(ptPageLayout, ptInputEvent);
}

/* aptDirContents��������iDirContentNumber��
 * �ӵ�iStartIndex�ʼ��ʾ, ��ʾ��һҳ
 */
static int GenerateBrowsePageDirAndFile(int iStartIndex, int iDirContentsNumber, PT_DirContent *aptDirContents, PT_VideoMem ptVideoMem)
{
	static T_PixelDatas tDirIconPixelDatas;
	static T_PixelDatas tFileIconPixelDatas;
    static int bGetDirFileBmp = 0;
	T_PixelDatas tOriginIconPixelDatas;
    int iError;
    int i, j, k = 0;
    int iDirContentIndex = iStartIndex;
    PT_PageLayout ptPageLayout = &g_tBrowsePageDirAndFileLayout;
	PT_Layout atLayout = ptPageLayout->atLayout;

    if (!bGetDirFileBmp)
    {
        /* ��Ŀ¼ͼ�ꡢ�ļ�ͼ������ڴ� */
        tDirIconPixelDatas.iBpp           = ptVideoMem->tPixelDatas.iBpp;
        tDirIconPixelDatas.aucPixelDatas = malloc(ptPageLayout->iMaxTotalBytes);
        if (tDirIconPixelDatas.aucPixelDatas == NULL)
        {
            return -1;
        }

        tFileIconPixelDatas.iBpp           = ptVideoMem->tPixelDatas.iBpp;
        tFileIconPixelDatas.aucPixelDatas = malloc(ptPageLayout->iMaxTotalBytes);
        if (tFileIconPixelDatas.aucPixelDatas == NULL)
        {
            return -1;
        }

        /* ��BMP�ļ�����ȡͼ������ */
        /* 1. ��ȡ"Ŀ¼ͼ��" */
        iError = GetPixelDatasForIcon(g_strDirIconName, &tOriginIconPixelDatas);
        if (iError)
        {
            DBG_PRINTF("GetPixelDatasForIcon %s error!\n", g_strDirIconName);
            return -1;
        }
        tDirIconPixelDatas.iHeight = atLayout[0].iBotRightY - atLayout[0].iTopLeftY + 1;
        tDirIconPixelDatas.iWidth  = atLayout[0].iBotRightX - atLayout[0].iTopLeftX + 1;
        tDirIconPixelDatas.iLineBytes  = tDirIconPixelDatas.iWidth * tDirIconPixelDatas.iBpp / 8;
        tDirIconPixelDatas.iTotalBytes = tDirIconPixelDatas.iLineBytes * tDirIconPixelDatas.iHeight;
        PicZoom(&tOriginIconPixelDatas, &tDirIconPixelDatas);
        FreePixelDatasForIcon(&tOriginIconPixelDatas);

        /* 2. ��ȡ"�ļ�ͼ��" */
        iError = GetPixelDatasForIcon(g_strFileIconName, &tOriginIconPixelDatas);
        if (iError)
        {
            DBG_PRINTF("GetPixelDatasForIcon %s error!\n", g_strFileIconName);
            return -1;
        }
        tFileIconPixelDatas.iHeight = atLayout[0].iBotRightY - atLayout[0].iTopLeftY + 1;
        tFileIconPixelDatas.iWidth  = atLayout[0].iBotRightX - atLayout[0].iTopLeftX+ 1;
        tFileIconPixelDatas.iLineBytes  = tDirIconPixelDatas.iWidth * tDirIconPixelDatas.iBpp / 8;
        tFileIconPixelDatas.iTotalBytes = tFileIconPixelDatas.iLineBytes * tFileIconPixelDatas.iHeight;
        PicZoom(&tOriginIconPixelDatas, &tFileIconPixelDatas);
        FreePixelDatasForIcon(&tOriginIconPixelDatas);

        bGetDirFileBmp = 1;
    }

    ClearRectangleInVideoMem(ptPageLayout->iTopLeftX, ptPageLayout->iTopLeftY, ptPageLayout->iBotRightX, ptPageLayout->iBotRightY, ptVideoMem, COLOR_BACKGROUND);

    SetFontSize(atLayout[1].iBotRightY - atLayout[1].iTopLeftY - 5);
    
    for (i = 0; i < g_iDirFileNumPerCol; i++)
    {
        for (j = 0; j < g_iDirFileNumPerRow; j++)
        {
            if (iDirContentIndex < iDirContentsNumber)
            {
                /* ��ʾĿ¼���ļ���ͼ�� */
                if (aptDirContents[iDirContentIndex]->eFileType == FILETYPE_DIR)
                {
                    PicMerge(atLayout[k].iTopLeftX, atLayout[k].iTopLeftY, &tDirIconPixelDatas, &ptVideoMem->tPixelDatas);
                }
                else
                {
                    PicMerge(atLayout[k].iTopLeftX, atLayout[k].iTopLeftY, &tFileIconPixelDatas, &ptVideoMem->tPixelDatas);
                }

                k++;
                /* ��ʾĿ¼���ļ������� */
                //DBG_PRINTF("MergerStringToCenterOfRectangleInVideoMem: %s\n", aptDirContents[iDirContentIndex]->strName);
                iError = MergerStringToCenterOfRectangleInVideoMem(atLayout[k].iTopLeftX, atLayout[k].iTopLeftY, atLayout[k].iBotRightX, atLayout[k].iBotRightY, (unsigned char *)aptDirContents[iDirContentIndex]->strName, ptVideoMem);
                //ClearRectangleInVideoMem(atLayout[k].iTopLeftX, atLayout[k].iTopLeftY, atLayout[k].iBotRightX, atLayout[k].iBotRightY, ptVideoMem, 0xff0000);
                k++;

                iDirContentIndex++;
            }
            else
            {
                break;
            }
        }
        if (iDirContentIndex >= iDirContentsNumber)
        {
            break;
        }
    }
	return 0;
}

static void ShowBrowsePage(PT_PageLayout ptPageLayout)
{
	PT_VideoMem ptVideoMem;
	int iError;

	PT_Layout atLayout = ptPageLayout->atLayout;
		
	/* 1. ����Դ� */
	ptVideoMem = GetVideoMem(ID("browse"), 1);
	if (ptVideoMem == NULL)
	{
		DBG_PRINTF("can't get video mem for browse page!\n");
		return;
	}

	/* 2. �軭���� */

	/* �����û�м������ͼ������� */
	if (atLayout[0].iTopLeftX == 0)
	{
		CalcBrowsePageMenusLayout(ptPageLayout);
        CalcBrowsePageDirAndFilesLayout();
	}

	iError = GeneratePage(ptPageLayout, ptVideoMem);
    iError = GenerateBrowsePageDirAndFile(g_iStartIndex, g_iDirContentsNumber, g_aptDirContents, ptVideoMem);

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
    int iError;
    PT_VideoMem ptDevVideoMem;

    ptDevVideoMem = GetDevVideoMem();

    /* 0. ���Ҫ��ʾ��Ŀ¼������ */
    iError = GetDirContents(g_strCurDir, &g_aptDirContents, &g_iDirContentsNumber);
    if (iError)
    {
        DBG_PRINTF("GetDirContents error!\n");
        return;
    }
    	
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
				ReleaseButton(&g_atBrowsePageMenusLayout[iIndexPressed]);
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
                        case 1: /* "ѡ��" */
                        {
                            break;
                        }
                        case 2: /* "��һҳ" */
                        {
                            g_iStartIndex -= g_iDirFileNumPerCol * g_iDirFileNumPerRow;
                            if (g_iStartIndex >= 0)
                            {
                                iError = GenerateBrowsePageDirAndFile(g_iStartIndex, g_iDirContentsNumber, g_aptDirContents, ptDevVideoMem);
                            }
                            else
                            {
                                g_iStartIndex += g_iDirFileNumPerCol * g_iDirFileNumPerRow;
                            }
                            break;
                        }
                        case 3: /* "��һҳ" */
                        {
                            g_iStartIndex += g_iDirFileNumPerCol * g_iDirFileNumPerRow;
                            if (g_iStartIndex < g_iDirContentsNumber)
                            {
                                iError = GenerateBrowsePageDirAndFile(g_iStartIndex, g_iDirContentsNumber, g_aptDirContents, ptDevVideoMem);
                            }
                            else
                            {
                                g_iStartIndex -= g_iDirFileNumPerCol * g_iDirFileNumPerRow;
                            }
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
					PressButton(&g_atBrowsePageMenusLayout[iIndexPressed]);
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

