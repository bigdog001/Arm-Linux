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

	/* 3. 刷到设备上去 */
	FlushVideoMemToDev(ptVideoMem);

	/* 4. 解放显存 */
	PutVideoMem(ptVideoMem);
}


/* manual页面的Run参数含义: 
 * NULL   - 显示browse页面, 
 * 非NULL - 文件名, 显示它
 */
static void ManualPageRun(void *pVoid)
{
	T_InputEvent tInputEvent;
    int iRet;
    
    if (NULL == pVoid)
    {
        /* browse页面的Run参数含义: 
         * 0 - 用于观看图片, 点击图片时会显示它
         * 1 - 用于浏览/选择文件夹, 点击文件无反应 
         */
        Page("browse")->Run((void *)0);
    }
    else
    {
        ShowManualPage(&g_tManualPageMenuIconsLayout, (char *)pVoid);

        /* 点击触摸屏才返回 */
        iRet = GetInputEvent(&tInputEvent);
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

