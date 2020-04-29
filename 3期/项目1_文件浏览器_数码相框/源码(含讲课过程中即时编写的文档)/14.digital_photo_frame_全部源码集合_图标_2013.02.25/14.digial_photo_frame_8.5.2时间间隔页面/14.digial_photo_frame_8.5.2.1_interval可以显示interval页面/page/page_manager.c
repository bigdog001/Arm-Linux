#include <config.h>
#include <page_manager.h>
#include <render.h>
#include <string.h>
#include <stdlib.h>


static PT_PageAction g_ptPageActionHead;

int RegisterPageAction(PT_PageAction ptPageAction)
{
	PT_PageAction ptTmp;

	if (!g_ptPageActionHead)
	{
		g_ptPageActionHead   = ptPageAction;
		ptPageAction->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptPageActionHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptPageAction;
		ptPageAction->ptNext = NULL;
	}

	return 0;
}


void ShowPages(void)
{
	int i = 0;
	PT_PageAction ptTmp = g_ptPageActionHead;

	while (ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

PT_PageAction Page(char *pcName)
{
	PT_PageAction ptTmp = g_ptPageActionHead;
	
	while (ptTmp)
	{
		if (strcmp(ptTmp->name, pcName) == 0)
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;
}

int ID(char *strName)
{
	return (int)(strName[0]) + (int)(strName[1]) + (int)(strName[2]) + (int)(strName[3]);
}

int GeneratePage(PT_PageLayout ptPageLayout, PT_VideoMem ptVideoMem)
{
	T_PixelDatas tOriginIconPixelDatas;
	T_PixelDatas tIconPixelDatas;
	int iError;
	PT_Layout atLayout = ptPageLayout->atLayout;
		
	/* Ãè»­Êý¾Ý */
	if (ptVideoMem->ePicState != PS_GENERATED)
	{
		ClearVideoMem(ptVideoMem, 0);

		tIconPixelDatas.iBpp          = ptPageLayout->iBpp;
		tIconPixelDatas.aucPixelDatas = malloc(ptPageLayout->iMaxTotalBytes);
		if (tIconPixelDatas.aucPixelDatas == NULL)
		{
			return -1;
		}

		while (atLayout->strIconName)
		{
			iError = GetPixelDatasForIcon(atLayout->strIconName, &tOriginIconPixelDatas);
			if (iError)
			{
				DBG_PRINTF("GetPixelDatasForIcon %s error!\n", atLayout->strIconName);
				return -1;
			}
 			tIconPixelDatas.iHeight = atLayout->iBotRightY - atLayout->iTopLeftY + 1;
			tIconPixelDatas.iWidth  = atLayout->iBotRightX - atLayout->iTopLeftX+ 1;
			tIconPixelDatas.iLineBytes  = tIconPixelDatas.iWidth * tIconPixelDatas.iBpp / 8;
			tIconPixelDatas.iTotalBytes = tIconPixelDatas.iLineBytes * tIconPixelDatas.iHeight;
 			PicZoom(&tOriginIconPixelDatas, &tIconPixelDatas);
 			PicMerge(atLayout->iTopLeftX, atLayout->iTopLeftY, &tIconPixelDatas, &ptVideoMem->tPixelDatas);
 			FreePixelDatasForIcon(&tOriginIconPixelDatas);
 			atLayout++;
		}
		free(tIconPixelDatas.aucPixelDatas);
		ptVideoMem->ePicState = PS_GENERATED;
	}

	return 0;
}

int PagesInit(void)
{
	int iError;

	iError = MainPageInit();
	iError |= SettingPageInit();
	iError |= IntervalPageInit();
		
	return iError;
}


