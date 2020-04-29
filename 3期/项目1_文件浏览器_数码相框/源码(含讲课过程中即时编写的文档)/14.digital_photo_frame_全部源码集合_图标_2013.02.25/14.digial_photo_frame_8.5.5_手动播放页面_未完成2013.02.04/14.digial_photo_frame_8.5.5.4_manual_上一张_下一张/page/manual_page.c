#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <file.h>
#include <fonts_manager.h>
#include <string.h>

/* �˵������� */
static T_Layout g_atManualMenuIconsLayout[] = {
	{0, 0, 0, 0, "return.bmp"},
	{0, 0, 0, 0, "zoomout.bmp"},
	{0, 0, 0, 0, "zoomin.bmp"},
	{0, 0, 0, 0, "pre_pic.bmp"},
    {0, 0, 0, 0, "next_pic.bmp"},
    {0, 0, 0, 0, "continue_mod_small.bmp"},
	{0, 0, 0, 0, NULL},
};

static T_PageLayout g_tManualPageMenuIconsLayout = {
	.iMaxTotalBytes = 0,
	.atLayout       = g_atManualMenuIconsLayout,
};

static T_Layout g_tManualPictureLayout;


/* ����˵��и�ͼ������ֵ */
static void  CalcManualPageMenusLayout(PT_PageLayout ptPageLayout)
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
		/*	 iXres/6
		 *	  --------------------------------------------------------------
		 *	   return	zoomout	zoomin  pre_pic next_pic continue_mod_small
		 *
		 *
		 *
		 *
		 *
		 *
		 *	  --------------------------------------------------------------
		 */
		 
		iWidth  = iXres / 6;
		iHeight = iWidth;

		/* returnͼ�� */
		atLayout[0].iTopLeftY  = 0;
		atLayout[0].iBotRightY = atLayout[0].iTopLeftY + iHeight - 1;
		atLayout[0].iTopLeftX  = 0;
		atLayout[0].iBotRightX = atLayout[0].iTopLeftX + iWidth - 1;

        /* ����5��ͼ�� */
        for (i = 1; i < 6; i++)
        {
    		atLayout[i].iTopLeftY  = 0;
    		atLayout[i].iBotRightY = atLayout[i].iTopLeftY + iHeight - 1;
    		atLayout[i].iTopLeftX  = atLayout[i-1].iBotRightX + 1;
    		atLayout[i].iBotRightX = atLayout[i].iTopLeftX + iWidth - 1;
        }

	}
	else
	{
		/*	 iYres/6
		 *	  ----------------------------------
		 *	   up		  
		 *
		 *    zoomout	    
		 *
		 *    zoomin
		 *  
		 *    pre_pic
		 *
		 *    next_pic
		 *
		 *    continue_mod_small
		 *
		 *	  ----------------------------------
		 */
		 
		iHeight  = iYres / 6;
		iWidth = iHeight;

		/* returnͼ�� */
		atLayout[0].iTopLeftY  = 0;
		atLayout[0].iBotRightY = atLayout[0].iTopLeftY + iHeight - 1;
		atLayout[0].iTopLeftX  = 0;
		atLayout[0].iBotRightX = atLayout[0].iTopLeftX + iWidth - 1;
		
        /* ����5��ͼ�� */
        for (i = 0; i < 6; i++)
        {
    		atLayout[i].iTopLeftY  = atLayout[i-1].iBotRightY+ 1;
    		atLayout[i].iBotRightY = atLayout[i].iTopLeftY + iHeight - 1;
    		atLayout[i].iTopLeftX  = 0;
    		atLayout[i].iBotRightX = atLayout[i].iTopLeftX + iWidth - 1;
        }		
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


/* ����ͼƬ����ʾ���� */
static void CalcManualPagePictureLayout(void)
{
	int iXres, iYres, iBpp;
	int iTopLeftX, iTopLeftY;
	int iBotRightX, iBotRightY;
	
	GetDispResolution(&iXres, &iYres, &iBpp);

	if (iXres < iYres)
	{
		/*	 iXres/6
		 *	  --------------------------------------------------------------
		 *	   return	zoomout	zoomin  pre_pic next_pic continue_mod_small  (ͼ��)
		 *	  --------------------------------------------------------------
		 *
		 *                              ͼƬ
		 *
		 *
		 *	  --------------------------------------------------------------
		 */
		iTopLeftX  = 0;
		iBotRightX = iXres - 1;
		iTopLeftY  = g_atManualMenuIconsLayout[0].iBotRightY + 1;
		iBotRightY = iYres - 1;
	}
	else
	{
		/*	 iYres/6
		 *	  --------------------------------------------------------------
		 *	   up		         |
		 *                       |
		 *    zoomout	         |
		 *                       |
		 *    zoomin             |
		 *                       |
		 *    pre_pic            |                 ͼƬ
		 *                       |
		 *    next_pic           |
		 *                       |
		 *    continue_mod_small | 
		 *                       |
		 *	  --------------------------------------------------------------
		 */
		iTopLeftX  = g_atManualMenuIconsLayout[0].iBotRightX + 1;
		iBotRightX = iXres - 1;
		iTopLeftY  = 0;
		iBotRightY = iYres - 1;
	}

    g_tManualPictureLayout.iTopLeftX   = iTopLeftX;
    g_tManualPictureLayout.iTopLeftY   = iTopLeftY;
    g_tManualPictureLayout.iBotRightX  = iBotRightX;
    g_tManualPictureLayout.iBotRightY  = iBotRightY;
    g_tManualPictureLayout.strIconName = NULL;
}

static int ShowPictureInManualPage(PT_VideoMem ptVideoMem, char *strFileName)
{
	int iError;
	T_PixelDatas tOriginIconPixelDatas;
	T_PixelDatas tPicPixelDatas;
    float k;
	int iXres, iYres, iBpp;
    int iPictureLayoutWidth;
    int iPictureLayoutHeight;
    int iTopLeftX, iTopLeftY;

	GetDispResolution(&iXres, &iYres, &iBpp);

    /* ���ͼƬ�ļ������� */
    iError = GetPixelDatasFrmFile(strFileName, &tOriginIconPixelDatas);
    if (iError)
    {
        return -1;
    }
    
    /* ��ͼƬ���������ŵ�LCD��Ļ��, ������ʾ */
    iPictureLayoutWidth  = g_tManualPictureLayout.iBotRightX - g_tManualPictureLayout.iTopLeftX + 1;
    iPictureLayoutHeight = g_tManualPictureLayout.iBotRightY - g_tManualPictureLayout.iTopLeftY + 1;
    k = (float)tOriginIconPixelDatas.iHeight / tOriginIconPixelDatas.iWidth;
    tPicPixelDatas.iWidth  = iPictureLayoutWidth;
    tPicPixelDatas.iHeight = iPictureLayoutWidth * k;
    if (tPicPixelDatas.iHeight > iPictureLayoutHeight)
    {
        tPicPixelDatas.iWidth  = iPictureLayoutHeight / k;
        tPicPixelDatas.iHeight = iPictureLayoutHeight;
    }
    tPicPixelDatas.iBpp        = iBpp;
    tPicPixelDatas.iLineBytes  = tPicPixelDatas.iWidth * tPicPixelDatas.iBpp / 8;
    tPicPixelDatas.iTotalBytes = tPicPixelDatas.iLineBytes * tPicPixelDatas.iHeight;
    tPicPixelDatas.aucPixelDatas = malloc(tPicPixelDatas.iTotalBytes);
    if (tPicPixelDatas.aucPixelDatas == NULL)
    {
        return -1;
    }
    
    PicZoom(&tOriginIconPixelDatas, &tPicPixelDatas);
    
    /* ���������ʾʱ���Ͻ����� */
    iTopLeftX = (iXres - tPicPixelDatas.iWidth) / 2;
    iTopLeftY = (iYres - tPicPixelDatas.iHeight) / 2;
    PicMerge(iTopLeftX, iTopLeftY, &tPicPixelDatas, &ptVideoMem->tPixelDatas);
    FreePixelDatasFrmFile(&tOriginIconPixelDatas);
    free(tPicPixelDatas.aucPixelDatas);

    return 0;
}

static void ShowManualPage(PT_PageLayout ptPageLayout, char *strFileName)
{
	PT_VideoMem ptVideoMem;
	int iError;

	PT_Layout atLayout = ptPageLayout->atLayout;
		
	/* 1. ����Դ� */
	ptVideoMem = GetVideoMem(ID("manual"), 1);
	if (ptVideoMem == NULL)
	{
		DBG_PRINTF("can't get video mem for manual page!\n");
		return;
	}

	/* 2. �軭���� */

	/* �����û�м������ͼ������� */
	if (atLayout[0].iTopLeftX == 0)
	{
		CalcManualPageMenusLayout(ptPageLayout);
        CalcManualPagePictureLayout();
	}

    /* ��videomem������ͼ�� */
	iError = GeneratePage(ptPageLayout, ptVideoMem);

    iError = ShowPictureInManualPage(ptVideoMem, strFileName);
    if (iError)
    {
        PutVideoMem(ptVideoMem);
        return;
    }
    
	/* 3. ˢ���豸��ȥ */
	FlushVideoMemToDev(ptVideoMem);

	/* 4. ����Դ� */
	PutVideoMem(ptVideoMem);
}

static int ManualPageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	return GenericGetInputEvent(ptPageLayout, ptInputEvent);
}


/* manualҳ���Run��������: 
 * ������ĸ�ҳ������ҳ��   - ��ʾbrowseҳ��, 
 * ���� - ��ʾͼƬ
 */
static void ManualPageRun(PT_PageParams ptParentPageParams)
{
	T_InputEvent tInputEvent;
	int bPressed = 0;
	int iIndexPressed = -1;
    int iIndex;
    T_PageParams tPageParams;
    int iError;
    char strDirName[256];
    char strFileName[256];
    char strFullPathName[256];
    PT_DirContent *aptDirContents;
    int iDirContentsNumber;
    int iPicFileIndex;
    char *pcTmp;
    PT_VideoMem ptDevVideoMem;

    tPageParams.iPageID = ID("manual");
    
    if (ptParentPageParams->iPageID == ID("main"))
    {
        /* ������ĸ�ҳ������ҳ�� - ��ʾbrowseҳ��*/
        Page("browse")->Run(ptParentPageParams);
    }
    else
    {
        ptDevVideoMem = GetDevVideoMem();
        strcpy(strFullPathName, ptParentPageParams->strCurPictureFile);
        ShowManualPage(&g_tManualPageMenuIconsLayout, strFullPathName);

        /* ȡ��Ŀ¼�� */
        strcpy(strDirName, ptParentPageParams->strCurPictureFile);
        pcTmp = strrchr(strDirName, '/');
        *pcTmp = '\0';

        /* ȡ���ļ��� */
        strcpy(strFileName, pcTmp+1);

        /* ��õ�ǰĿ¼������Ŀ¼���ļ������� */
        iError = GetDirContents(strDirName, &aptDirContents, &iDirContentsNumber);

        /* ȷ����ǰ��ʾ������һ���ļ� */
        for (iPicFileIndex = 0; iPicFileIndex < iDirContentsNumber; iPicFileIndex++)
        {
            if (0 == strcmp(strFileName, aptDirContents[iPicFileIndex]->strName))
            {
                break;
            }
        }

        while (1)
        {
            /* ��ȷ���Ƿ����˲˵�ͼ�� */
            iIndex = ManualPageGetInputEvent(&g_tManualPageMenuIconsLayout, &tInputEvent);
            if (tInputEvent.iPressure == 0)
            {
                /* ������ɿ� */
                if (bPressed)
                {
                    /* �����а�ť������ */
                    ReleaseButton(&g_atManualMenuIconsLayout[iIndexPressed]);
                    bPressed = 0;

                    if (iIndexPressed == iIndex) /* ���º��ɿ�����ͬһ����ť */
                    {
                        switch (iIndexPressed)
                        {
                            case 0: /* ���ذ�ť */
                            {
                                return;
                                break;
                            }
                            case 1: /* ��С��ť */
                            {
                                break;
                            }
                            case 2: /* �Ŵ�ť */
                            {

                                break;
                            }
                            case 3: /* "��һ��"��ť */
                            {
                                while (iPicFileIndex > 0)
                                {
                                    iPicFileIndex--;
                                    snprintf(strFullPathName, 256, "%s/%s", strDirName, aptDirContents[iPicFileIndex]->strName);
                                    strFullPathName[255] = '\0';
                                    
                                    if (isPictureFileSupported(strFullPathName))
                                    {
                                        ShowPictureInManualPage(ptDevVideoMem, strFullPathName);
                                        break;
                                    }
                                }
                                
                                break;
                            }
                            case 4: /* "��һ��"��ť */
                            {
                                while (iPicFileIndex < iDirContentsNumber - 1)
                                {
                                    iPicFileIndex++;
                                    snprintf(strFullPathName, 256, "%s/%s", strDirName, aptDirContents[iPicFileIndex]->strName);
                                    strFullPathName[255] = '\0';

                                    if (isPictureFileSupported(strFullPathName))
                                    {
                                        ShowPictureInManualPage(ptDevVideoMem, strFullPathName);
                                        break;
                                    }
                                }
                                break;
                            }
                            case 5: /* "����"��ť */
                            {
                                /* Manualҳ��Ĵ�������������: ����ҳ�水"���ģʽ"����"���ҳ��"->"ѡ��ĳ���ļ�", ��"����ҳ��"����������ʾ��ͼƬ 
                                 * ����Ǻ���, ֱ��return�Ϳ�����:��Ϊreturn���Ƿ��ص�"����ҳ��"��, �������"����"
                                 */
                                DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
                                if (ptParentPageParams->iPageID == ID("browse"))  /* ������"���ҳ��" */
                                {
                                    DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
                                    strcpy(tPageParams.strCurPictureFile, strFullPathName);
                                    Page("auto")->Run(&tPageParams);  /* autoҳ�淵��ǰ,�����������ʾ���ļ�����tPageParams.strCurPictureFile */
                                    ShowManualPage(&g_tManualPageMenuIconsLayout, tPageParams.strCurPictureFile);
                                }
                                else /* ��ǰmanualҳ��ĸ�ҳ����autoҳ��, ֱ�ӷ��ؼ��� */
                                {
                                    DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
                                    return;
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
                        PressButton(&g_atManualMenuIconsLayout[iIndexPressed]);
                    }
                }
            }       

            /* ����������ڲ˵�ͼ����*/
            if (iIndex == -1)
            {
            }
            else /* ������㲻�ڲ˵�ͼ����, �������ȡ����������, �Ӷ�����ͼƬ */
            {
            }
        }
    }
}

static T_PageAction g_tManualPageAction = {
	.name          = "manual",
	.Run           = ManualPageRun,
};

int ManualPageInit(void)
{
	return RegisterPageAction(&g_tManualPageAction);
}

