#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <file.h>
#include <string.h>


static void AutoPageRun(void)
{
    /* 1. 启动一个线程来连续显示图片 */

    /* 2. 当前线程等待触摸屏输入, 先做简单点: 如果点击了触摸屏, 让线程退出 */

    /* 测试用 */
	T_PixelDatas tOriginIconPixelDatas;
	T_PixelDatas tPicPixelDatas;
    PT_VideoMem ptVideoMem;
	int iError;
	int iXres, iYres, iBpp;
    int iTopLeftX, iTopLeftY;
    float k;
	T_InputEvent tInputEvent;
	int iRet;

	GetDispResolution(&iXres, &iYres, &iBpp);

	/* 获得显存 */
	ptVideoMem = GetVideoMem(ID("auto"), 1);
	if (ptVideoMem == NULL)
	{
		DBG_PRINTF("can't get video mem for browse page!\n");
		return;
	}
    ClearVideoMem(ptVideoMem, COLOR_BACKGROUND);
    
    iError = GetPixelDatasFrmFile("/1.jpg", &tOriginIconPixelDatas);
    if (iError)
    {
        DBG_PRINTF("GetPixelDatasFrmFile error: /1.jpg\n");
        return;
    }

    /* 把图片按比例缩放到LCD屏幕上, 居中显示 */
    k = (float)tOriginIconPixelDatas.iHeight / tOriginIconPixelDatas.iWidth;
    tPicPixelDatas.iWidth  = iXres;
    tPicPixelDatas.iHeight = iXres * k;
    if (tPicPixelDatas.iHeight > iYres)
    {
        tPicPixelDatas.iWidth  = iYres / k;
        tPicPixelDatas.iHeight = iYres;
    }
    tPicPixelDatas.iBpp        = iBpp;
    tPicPixelDatas.iLineBytes  = tPicPixelDatas.iWidth * tPicPixelDatas.iBpp / 8;
    tPicPixelDatas.iTotalBytes = tPicPixelDatas.iLineBytes * tPicPixelDatas.iHeight;
    tPicPixelDatas.aucPixelDatas = malloc(tPicPixelDatas.iTotalBytes);
    if (tPicPixelDatas.aucPixelDatas == NULL)
    {
        return;
    }

    PicZoom(&tOriginIconPixelDatas, &tPicPixelDatas);

    /* 算出居中显示时左上角坐标 */
    iTopLeftX = (iXres - tPicPixelDatas.iWidth) / 2;
    iTopLeftY = (iYres - tPicPixelDatas.iHeight) / 2;
    PicMerge(iTopLeftX, iTopLeftY, &tPicPixelDatas, &ptVideoMem->tPixelDatas);
    FreePixelDatasFrmFile(&tOriginIconPixelDatas);
    free(tPicPixelDatas.aucPixelDatas);
    
	/* 刷到设备上去 */
	FlushVideoMemToDev(ptVideoMem);

	/* 解放显存 */
	PutVideoMem(ptVideoMem);

    iRet = GetInputEvent(&tInputEvent);
}

static T_PageAction g_tAutoPageAction = {
	.name          = "auto",
	.Run           = AutoPageRun,
};

int AutoPageInit(void)
{
	return RegisterPageAction(&g_tAutoPageAction);
}


