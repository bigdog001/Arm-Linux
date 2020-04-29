#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <file.h>
#include <string.h>


static void AutoPageRun(void)
{
    /* 1. ����һ���߳���������ʾͼƬ */

    /* 2. ��ǰ�̵߳ȴ�����������, �����򵥵�: �������˴�����, ���߳��˳� */

    /* ������ */
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

	/* ����Դ� */
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

    /* ��ͼƬ���������ŵ�LCD��Ļ��, ������ʾ */
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

    /* ���������ʾʱ���Ͻ����� */
    iTopLeftX = (iXres - tPicPixelDatas.iWidth) / 2;
    iTopLeftY = (iYres - tPicPixelDatas.iHeight) / 2;
    PicMerge(iTopLeftX, iTopLeftY, &tPicPixelDatas, &ptVideoMem->tPixelDatas);
    FreePixelDatasFrmFile(&tOriginIconPixelDatas);
    free(tPicPixelDatas.aucPixelDatas);
    
	/* ˢ���豸��ȥ */
	FlushVideoMemToDev(ptVideoMem);

	/* ����Դ� */
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


