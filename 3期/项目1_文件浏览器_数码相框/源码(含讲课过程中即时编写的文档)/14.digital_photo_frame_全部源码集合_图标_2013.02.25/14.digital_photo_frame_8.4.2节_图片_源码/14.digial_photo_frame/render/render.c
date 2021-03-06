#include <stdio.h>


extern T_PicFileParser g_tBMPParser;


int GetFontPixel()
{
}

int GetPicPixel()
{
}


int DrawPixel()
{
}

void FlushVideoMemToDev(PT_VideoMem ptVideoMem)
{
	//memcpy(GetDefaultDispDev()->pucDispMem, ptVideoMem->tPixelDatas.aucPixelDatas, ptVideoMem.tPixelDatas.iHeight * ptVideoMem.tPixelDatas.iLineBytes);
	if (!ptVideoMem->bDevFrameBuffer)
	{
		GetDefaultDispDev()->ShowPage(ptVideoMem);
	}
}

int GetPixelDatasForIcon(char *strFileName, PT_PixelDatas ptPixelDatas)
{
	T_FileMap tFileMap;
	int iError;
	int iXres, iYres, iBpp;

	/* ͼ����� /etc/digitpic/icons */
	snprintf(tFileMap.strFileName, 128, "%s/%s", ICON_PATH, strFileName);
	tFileMap.strFileName[127] = '\0';
	
	iError = MapFile(&tFileMap);
	if (iError)
	{
		DBG_PRINTF("MapFile %s error!\n", strFileName);
		return -1;
	}

	iError = g_tBMPParser->isSupport(tFileMap.pucFileMapMem);
	if (iError == 0)
	{
		DBG_PRINTF("%s is not bmp file\n", strFileName);
		return -1;
	}

	GetDispResolution(&iXres, &iYres, &iBpp);
	ptPixelDatas->iBpp = iBpp;
	iError = g_tBMPParser->GetPixelDatas(tFileMap.pucFileMapMem, ptPixelDatas);
	if (iError)
	{
		DBG_PRINTF("GetPixelDatas for %s error!\n", strFileName);
		return -1;
	}

	return 0;
}

void FreePixelDatasForIcon(PT_PixelDatas ptPixelDatas)
{
	g_tBMPParser->FreePixelDatas(ptPixelDatas);
}



