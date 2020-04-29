#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <file.h>
#include <fonts_manager.h>
#include <string.h>

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

    /* 获得图片文件的数据 */
    iError = GetPixelDatasFrmFile(strFileName, &tOriginIconPixelDatas);
    if (iError)
    {
        return -1;
    }
    
    /* 把图片按比例缩放到LCD屏幕上, 居中显示 */
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
    
    /* 算出居中显示时左上角坐标 */
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
        ShowManualPage(&g_tManualPageMenuIconsLayout, strFullPathName);

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
                                break;
                            }
                            case 2: /* 放大按钮 */
                            {

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

