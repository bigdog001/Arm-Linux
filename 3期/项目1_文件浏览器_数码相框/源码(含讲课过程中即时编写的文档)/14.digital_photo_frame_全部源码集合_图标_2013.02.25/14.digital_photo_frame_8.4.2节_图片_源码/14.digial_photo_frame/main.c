#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <config.h>
#include <draw.h>
#include <encoding_manager.h>
#include <fonts_manager.h>
#include <disp_manager.h>
#include <input_manager.h>
#include <pic_operation.h>
#include <render.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


/* digitpic <bmp_file> */
int main(int argc, char **argv)
{
	int iFdBmp;
	int iRet;
	unsigned char *pucBMPmem;
	struct stat tBMPstat;
		
	PT_DispOpr ptDispOpr;

	extern T_PicFileParser g_tBMPParser;

	T_PixelDatas tPixelDatas;
	T_PixelDatas tPixelDatasSmall;

	T_PixelDatas tPixelDatasFB;

	if (argc != 2)
	{
		printf("%s <bmp_file>\n", argv[0]);
		return -1;
	}

	DebugInit();
	InitDebugChanel();

	DisplayInit();

	ptDispOpr = GetDispOpr("fb");
	ptDispOpr->DeviceInit();
	ptDispOpr->CleanScreen(0);
	

	/* 打开BMP文件 */
	iFdBmp = open(argv[1], O_RDWR);
	if (iFdBmp == -1)
	{
		DBG_PRINTF("can't open %s\n", argv[1]);
	}

	fstat(iFdBmp, &tBMPstat);
	pucBMPmem = (unsigned char *)mmap(NULL , tBMPstat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, iFdBmp, 0);
	if (pucBMPmem == (unsigned char *)-1)
	{
		DBG_PRINTF("mmap error!\n");
		return -1;
	}

	/* 提取BMP文件的RGB数据, 缩放, 在LCD上显示出来 */
	iRet = g_tBMPParser.isSupport(pucBMPmem);
	if (iRet == 0)
	{
		DBG_PRINTF("%s is not bmp file\n", argv[1]);
		return -1;		
	}

	tPixelDatas.iBpp = ptDispOpr->iBpp;
	iRet = g_tBMPParser.GetPixelDatas(pucBMPmem, &tPixelDatas);
	if (iRet)
	{
		DBG_PRINTF("GetPixelDatas error!\n");
		return -1;		
	}

	tPixelDatasFB.iWidth        = ptDispOpr->iXres;
	tPixelDatasFB.iHeight       = ptDispOpr->iYres;
	tPixelDatasFB.iBpp          = ptDispOpr->iBpp;
	tPixelDatasFB.iLineBytes    = ptDispOpr->iXres * ptDispOpr->iBpp / 8; 
	tPixelDatasFB.aucPixelDatas = ptDispOpr->pucDispMem;
	
	PicMerge(0, 0, &tPixelDatas, &tPixelDatasFB);

	tPixelDatasSmall.iWidth  = tPixelDatas.iWidth/2;
	tPixelDatasSmall.iHeight = tPixelDatas.iHeight/2;
	tPixelDatasSmall.iBpp    = tPixelDatas.iBpp;
	tPixelDatasSmall.iLineBytes = tPixelDatasSmall.iWidth * tPixelDatasSmall.iBpp / 8;
	tPixelDatasSmall.aucPixelDatas = malloc(tPixelDatasSmall.iLineBytes * tPixelDatasSmall.iHeight);
	
	PicZoom(&tPixelDatas, &tPixelDatasSmall);
	PicMerge(128, 128, &tPixelDatasSmall, &tPixelDatasFB);
	
		
	return 0;
}

