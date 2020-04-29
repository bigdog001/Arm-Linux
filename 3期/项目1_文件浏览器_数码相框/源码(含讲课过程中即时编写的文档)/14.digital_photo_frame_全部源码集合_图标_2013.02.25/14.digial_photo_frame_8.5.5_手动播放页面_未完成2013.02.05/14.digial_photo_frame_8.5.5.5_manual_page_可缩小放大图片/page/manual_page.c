#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <file.h>
#include <fonts_manager.h>
#include <string.h>

/* 放大/缩小系数 */
#define ZOOM_RATIO (0.9)

/* 菜单的区域 */
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

static T_PixelDatas g_tOriginPicPixelDatas;
static T_PixelDatas g_tZoomedPicPixelDatas;

/* 显示在LCD上的图片, 它的中心点, 在g_tZoomedPicPixelDatas里的坐标 */
static int g_iXofZoomedPicShowInCenter;  
static int g_iYofZoomedPicShowInCenter;


/* 计算菜单中各图标坐标值 */
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

		/* return图标 */
		atLayout[0].iTopLeftY  = 0;
		atLayout[0].iBotRightY = atLayout[0].iTopLeftY + iHeight - 1;
		atLayout[0].iTopLeftX  = 0;
		atLayout[0].iBotRightX = atLayout[0].iTopLeftX + iWidth - 1;

        /* 其他5个图标 */
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

		/* return图标 */
		atLayout[0].iTopLeftY  = 0;
		atLayout[0].iBotRightY = atLayout[0].iTopLeftY + iHeight - 1;
		atLayout[0].iTopLeftX  = 0;
		atLayout[0].iBotRightX = atLayout[0].iTopLeftX + iWidth - 1;
		
        /* 其他5个图标 */
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


/* 计算图片的显示区域 */
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
		 *	   return	zoomout	zoomin  pre_pic next_pic continue_mod_small  (图标)
		 *	  --------------------------------------------------------------
		 *
		 *                              图片
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
		 *    pre_pic            |                 图片
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

static PT_PixelDatas GetOriginPictureFilePixelDatas(char *strFileName)
{
    int iError;

    if (g_tOriginPicPixelDatas.aucPixelDatas)
    {
        free(g_tOriginPicPixelDatas.aucPixelDatas);
        g_tOriginPicPixelDatas.aucPixelDatas = NULL;
    }
    
    /* 获得图片文件的数据 */
    iError = GetPixelDatasFrmFile(strFileName, &g_tOriginPicPixelDatas);
    if (iError)
    {
        return NULL;
    }
    else
    {
        return &g_tOriginPicPixelDatas;
    }
}

static PT_PixelDatas GetZoomedPicPixelDatas(PT_PixelDatas ptOriginPicPixelDatas, int iZoomedWidth, int iZoomedHeight)
{
    float k;
	int iXres, iYres, iBpp;
    
	GetDispResolution(&iXres, &iYres, &iBpp);

    if (g_tZoomedPicPixelDatas.aucPixelDatas)
    {
        free(g_tZoomedPicPixelDatas.aucPixelDatas);
        g_tZoomedPicPixelDatas.aucPixelDatas = NULL;
    }
    
    k = (float)ptOriginPicPixelDatas->iHeight / ptOriginPicPixelDatas->iWidth;
    g_tZoomedPicPixelDatas.iWidth  = iZoomedWidth;
    g_tZoomedPicPixelDatas.iHeight = iZoomedWidth * k;
    if (g_tZoomedPicPixelDatas.iHeight > iZoomedHeight)
    {
        g_tZoomedPicPixelDatas.iWidth  = iZoomedHeight / k;
        g_tZoomedPicPixelDatas.iHeight = iZoomedHeight;
    }
    g_tZoomedPicPixelDatas.iBpp        = iBpp;
    g_tZoomedPicPixelDatas.iLineBytes  = g_tZoomedPicPixelDatas.iWidth * g_tZoomedPicPixelDatas.iBpp / 8;
    g_tZoomedPicPixelDatas.iTotalBytes = g_tZoomedPicPixelDatas.iLineBytes * g_tZoomedPicPixelDatas.iHeight;
    g_tZoomedPicPixelDatas.aucPixelDatas = malloc(g_tZoomedPicPixelDatas.iTotalBytes);
    if (g_tZoomedPicPixelDatas.aucPixelDatas == NULL)
    {
        return NULL;
    }
    
    PicZoom(ptOriginPicPixelDatas, &g_tZoomedPicPixelDatas);
    return &g_tZoomedPicPixelDatas;
}

static int ShowPictureInManualPage(PT_VideoMem ptVideoMem, char *strFileName)
{
    PT_PixelDatas ptOriginPicPixelDatas;
    PT_PixelDatas ptZoomedPicPixelDatas;
    int iPictureLayoutWidth;
    int iPictureLayoutHeight;
    int iTopLeftX, iTopLeftY;
	int iXres, iYres, iBpp;

	GetDispResolution(&iXres, &iYres, &iBpp);

    /* 获得图片文件的原始数据 */
    ptOriginPicPixelDatas = GetOriginPictureFilePixelDatas(strFileName);
    if (!ptOriginPicPixelDatas)
    {
        return -1;
    }
    
    /* 把图片按比例缩放到LCD屏幕上, 居中显示 */
    iPictureLayoutWidth  = g_tManualPictureLayout.iBotRightX - g_tManualPictureLayout.iTopLeftX + 1;
    iPictureLayoutHeight = g_tManualPictureLayout.iBotRightY - g_tManualPictureLayout.iTopLeftY + 1;

    ptZoomedPicPixelDatas = GetZoomedPicPixelDatas(&g_tOriginPicPixelDatas, iPictureLayoutWidth, iPictureLayoutHeight);
    if (!ptZoomedPicPixelDatas)
    {
        return -1;
    }
        
    /* 算出居中显示时左上角坐标 */
    iTopLeftX = g_tManualPictureLayout.iTopLeftX + (iPictureLayoutWidth - ptZoomedPicPixelDatas->iWidth) / 2;
    iTopLeftY = g_tManualPictureLayout.iTopLeftY + (iPictureLayoutHeight - ptZoomedPicPixelDatas->iHeight) / 2;
    g_iXofZoomedPicShowInCenter = ptZoomedPicPixelDatas->iWidth / 2;
    g_iYofZoomedPicShowInCenter = ptZoomedPicPixelDatas->iHeight / 2;

    ClearVideoMemRegion(ptVideoMem, &g_tManualPictureLayout, COLOR_BACKGROUND);
    PicMerge(iTopLeftX, iTopLeftY, ptZoomedPicPixelDatas, &ptVideoMem->tPixelDatas);

    return 0;
}

static void ShowManualPage(PT_PageLayout ptPageLayout, char *strFileName)
{
	PT_VideoMem ptVideoMem;
	int iError;

	PT_Layout atLayout = ptPageLayout->atLayout;
		
	/* 1. 获得显存 */
	ptVideoMem = GetVideoMem(ID("manual"), 1);
	if (ptVideoMem == NULL)
	{
		DBG_PRINTF("can't get video mem for manual page!\n");
		return;
	}

	/* 2. 描画数据 */

	/* 如果还没有计算过各图标的坐标 */
	if (atLayout[0].iTopLeftX == 0)
	{
		CalcManualPageMenusLayout(ptPageLayout);
        CalcManualPagePictureLayout();
	}

    /* 在videomem上生成图标 */
	iError = GeneratePage(ptPageLayout, ptVideoMem);

    iError = ShowPictureInManualPage(ptVideoMem, strFileName);
    if (iError)
    {
        PutVideoMem(ptVideoMem);
        return;
    }
    
	/* 3. 刷到设备上去 */
	FlushVideoMemToDev(ptVideoMem);

	/* 4. 解放显存 */
	PutVideoMem(ptVideoMem);
}

static int ManualPageGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	return GenericGetInputEvent(ptPageLayout, ptInputEvent);
}


/* manual页面的Run参数含义: 
 * 如果它的父页面是主页面   - 显示browse页面, 
 * 否则 - 显示图片
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
    int iZoomedWidth;
    int iZoomedHeight;
    PT_PixelDatas ptZoomedPicPixelDatas;
    int iStartXofNewPic, iStartYofNewPic;
    int iStartXofOldPic, iStartYofOldPic;
    int iWidthPictureInPlay, iHeightPictureInPlay;
    int iPictureLayoutWidth, iPictureLayoutHeight;
	int iXres, iYres, iBpp;

	GetDispResolution(&iXres, &iYres, &iBpp);

    tPageParams.iPageID = ID("manual");
    
    if (ptParentPageParams->iPageID == ID("main"))
    {
        /* 如果它的父页面是主页面 - 显示browse页面*/
        Page("browse")->Run(ptParentPageParams);
    }
    else
    {
        ptDevVideoMem = GetDevVideoMem();
        strcpy(strFullPathName, ptParentPageParams->strCurPictureFile);

        /* 显示菜单和图片文件 */
        ShowManualPage(&g_tManualPageMenuIconsLayout, strFullPathName);

        iPictureLayoutWidth  = g_tManualPictureLayout.iBotRightX - g_tManualPictureLayout.iTopLeftX + 1;
        iPictureLayoutHeight = g_tManualPictureLayout.iBotRightY - g_tManualPictureLayout.iTopLeftY + 1;

        /* 取出目录名 */
        strcpy(strDirName, ptParentPageParams->strCurPictureFile);
        pcTmp = strrchr(strDirName, '/');
        *pcTmp = '\0';

        /* 取出文件名 */
        strcpy(strFileName, pcTmp+1);

        /* 获得当前目录下所有目录和文件的名字 */
        iError = GetDirContents(strDirName, &aptDirContents, &iDirContentsNumber);

        /* 确定当前显示的是哪一个文件 */
        for (iPicFileIndex = 0; iPicFileIndex < iDirContentsNumber; iPicFileIndex++)
        {
            if (0 == strcmp(strFileName, aptDirContents[iPicFileIndex]->strName))
            {
                break;
            }
        }

        while (1)
        {
            /* 先确定是否触摸了菜单图标 */
            iIndex = ManualPageGetInputEvent(&g_tManualPageMenuIconsLayout, &tInputEvent);
            if (tInputEvent.iPressure == 0)
            {
                /* 如果是松开 */
                if (bPressed)
                {
                    /* 曾经有按钮被按下 */
                    ReleaseButton(&g_atManualMenuIconsLayout[iIndexPressed]);
                    bPressed = 0;

                    if (iIndexPressed == iIndex) /* 按下和松开都是同一个按钮 */
                    {
                        switch (iIndexPressed)
                        {
                            case 0: /* 返回按钮 */
                            {
                                return;
                                break;
                            }
                            case 1: /* 缩小按钮 */
                            {
                                /* 获得缩小后的数据 */
                                iZoomedWidth  = (float)g_tZoomedPicPixelDatas.iWidth * ZOOM_RATIO;
                                iZoomedHeight = (float)g_tZoomedPicPixelDatas.iHeight * ZOOM_RATIO;
                                ptZoomedPicPixelDatas = GetZoomedPicPixelDatas(&g_tOriginPicPixelDatas, iZoomedWidth, iZoomedHeight);

                                /* 重新计算中心点 */
                                g_iXofZoomedPicShowInCenter = (float)g_iXofZoomedPicShowInCenter * ZOOM_RATIO;
                                g_iYofZoomedPicShowInCenter = (float)g_iYofZoomedPicShowInCenter * ZOOM_RATIO;

                                /* 显示新数据 */
                                iStartXofNewPic = g_iXofZoomedPicShowInCenter - iPictureLayoutWidth/2;
                                if (iStartXofNewPic < 0)
                                {
                                    iStartXofNewPic = 0;
                                }
                                if (ptZoomedPicPixelDatas->iWidth - iStartXofNewPic > iPictureLayoutWidth)
                                    iWidthPictureInPlay = iPictureLayoutWidth;
                                else
                                    iWidthPictureInPlay = ptZoomedPicPixelDatas->iWidth - iStartXofNewPic;
                                
                                iStartYofNewPic = g_iYofZoomedPicShowInCenter - iPictureLayoutHeight/2;
                                if (iStartYofNewPic < 0)
                                {
                                    iStartYofNewPic = 0;
                                }
                                if (ptZoomedPicPixelDatas->iHeight - iStartYofNewPic > iPictureLayoutHeight)
                                    iHeightPictureInPlay = iPictureLayoutHeight;
                                else
                                    iHeightPictureInPlay = ptZoomedPicPixelDatas->iHeight - iStartYofNewPic;

                                iStartXofOldPic = g_tManualPictureLayout.iTopLeftX + (iPictureLayoutWidth - iWidthPictureInPlay) / 2;
                                iStartYofOldPic = g_tManualPictureLayout.iTopLeftY + (iPictureLayoutHeight - iHeightPictureInPlay) / 2;

                                ClearVideoMemRegion(ptDevVideoMem, &g_tManualPictureLayout, COLOR_BACKGROUND);
                                PicMergeRegion(iStartXofNewPic, iStartYofNewPic, iStartXofOldPic, iStartYofOldPic, iWidthPictureInPlay, iHeightPictureInPlay, ptZoomedPicPixelDatas, &ptDevVideoMem->tPixelDatas);

                                break;
                            }
                            case 2: /* 放大按钮 */
                            {
                                /* 获得放大后的数据 */
                                iZoomedWidth  = (float)g_tZoomedPicPixelDatas.iWidth / ZOOM_RATIO;
                                iZoomedHeight = (float)g_tZoomedPicPixelDatas.iHeight / ZOOM_RATIO;
                                ptZoomedPicPixelDatas = GetZoomedPicPixelDatas(&g_tOriginPicPixelDatas, iZoomedWidth, iZoomedHeight);

                                /* 重新计算中心点 */
                                g_iXofZoomedPicShowInCenter = (float)g_iXofZoomedPicShowInCenter / ZOOM_RATIO;
                                g_iYofZoomedPicShowInCenter = (float)g_iYofZoomedPicShowInCenter / ZOOM_RATIO;

                                /* 显示新数据 */
                                iStartXofNewPic = g_iXofZoomedPicShowInCenter - iPictureLayoutWidth/2;
                                if (iStartXofNewPic < 0)
                                {
                                    iStartXofNewPic = 0;
                                }
                                if (ptZoomedPicPixelDatas->iWidth - iStartXofNewPic > iPictureLayoutWidth)
                                    iWidthPictureInPlay = iPictureLayoutWidth;
                                else
                                    iWidthPictureInPlay = ptZoomedPicPixelDatas->iWidth - iStartXofNewPic;
                                
                                iStartYofNewPic = g_iYofZoomedPicShowInCenter - iPictureLayoutHeight/2;
                                if (iStartYofNewPic < 0)
                                {
                                    iStartYofNewPic = 0;
                                }
                                if (ptZoomedPicPixelDatas->iHeight - iStartYofNewPic > iPictureLayoutHeight)
                                    iHeightPictureInPlay = iPictureLayoutHeight;
                                else
                                    iHeightPictureInPlay = ptZoomedPicPixelDatas->iHeight - iStartYofNewPic;

                                iStartXofOldPic = g_tManualPictureLayout.iTopLeftX + (iPictureLayoutWidth - iWidthPictureInPlay) / 2;
                                iStartYofOldPic = g_tManualPictureLayout.iTopLeftY + (iPictureLayoutHeight - iHeightPictureInPlay) / 2;
                                
                                ClearVideoMemRegion(ptDevVideoMem, &g_tManualPictureLayout, COLOR_BACKGROUND);
                                PicMergeRegion(iStartXofNewPic, iStartYofNewPic, iStartXofOldPic, iStartYofOldPic, iWidthPictureInPlay, iHeightPictureInPlay, ptZoomedPicPixelDatas, &ptDevVideoMem->tPixelDatas);

                                break;
                            }
                            case 3: /* "上一张"按钮 */
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
                            case 4: /* "下一张"按钮 */
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
                            case 5: /* "连播"按钮 */
                            {
                                /* Manual页面的触发有两个方法: 在主页面按"浏览模式"进入"浏览页面"->"选中某个文件", 在"连播页面"里点击正在显示的图片 
                                 * 如果是后者, 直接return就可以了:因为return后是返回到"连播页面"的, 它会继续"连播"
                                 */
                                DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
                                if (ptParentPageParams->iPageID == ID("browse"))  /* 触发自"浏览页面" */
                                {
                                    DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
                                    strcpy(tPageParams.strCurPictureFile, strFullPathName);
                                    Page("auto")->Run(&tPageParams);  /* auto页面返回前,会把它正在显示的文件存在tPageParams.strCurPictureFile */
                                    ShowManualPage(&g_tManualPageMenuIconsLayout, tPageParams.strCurPictureFile);
                                }
                                else /* 当前manual页面的父页面是auto页面, 直接返回即可 */
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
                /* 按下状态 */
                if (iIndex != -1)
                {
                    if (!bPressed)
                    {
                        /* 未曾按下按钮 */
                        bPressed = 1;
                        iIndexPressed = iIndex;
                        PressButton(&g_atManualMenuIconsLayout[iIndexPressed]);
                    }
                }
            }       

            /* 如果触点是在菜单图标上*/
            if (iIndex == -1)
            {
            }
            else /* 如果触点不在菜单图标上, 则继续读取触摸屏输入, 从而滑动图片 */
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

