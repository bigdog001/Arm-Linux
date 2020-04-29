
#include <config.h>
#include <disp_manager.h>
#include <string.h>

static PT_DispOpr g_ptDispOprHead;
static PT_DispOpr g_ptDefaultDispOpr;
static PT_VideoMem g_ptVideoMemHead;


int RegisterDispOpr(PT_DispOpr ptDispOpr)
{
	PT_DispOpr ptTmp;

	if (!g_ptDispOprHead)
	{
		g_ptDispOprHead   = ptDispOpr;
		ptDispOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptDispOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptDispOpr;
		ptDispOpr->ptNext = NULL;
	}

	return 0;
}


void ShowDispOpr(void)
{
	int i = 0;
	PT_DispOpr ptTmp = g_ptDispOprHead;

	while (ptTmp)
	{
		printf("%02d %s\n", i++, ptTmp->name);
		ptTmp = ptTmp->ptNext;
	}
}

PT_DispOpr GetDispOpr(char *pcName)
{
	PT_DispOpr ptTmp = g_ptDispOprHead;
	
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

void SelectDefaultDispDev(char *name)
{
	g_ptDefaultDispOpr = GetDispOpr(name);
}

PT_DispOpr GetDefaultDispDev(void)
{
	return g_ptDefaultDispOpr;
}

int GetDispResolution(int *piXres, int *piYres, int *piBpp)
{
	if (g_ptDefaultDispOpr)
	{
		*piXres = g_ptDefaultDispOpr->iXres;
		*piYres = g_ptDefaultDispOpr->iYres;
		*piBpp  = g_ptDefaultDispOpr->iBpp;
		return 0;
	}
	else
	{
		return -1;
	}
}

/*  */
int AllocVideoMem(int iNum)
{
	int i;

	int iXres;
	int iYres;
	int iBpp;

	int iVMSize;
	int iLineBytes;

	PT_VideoMem ptNew;
		
	GetDispResolution(&iXres, &iYres, &iBpp);
	iVMSize = iXres * iYres * iBpp / 8;
	iLineBytes = iXres * iBpp / 8;

	/* �Ȱ��豸�����framebuffer�������� */
	ptNew = malloc(sizeof(T_VideoMem));
	if (ptNew == NULL)
	{
		return -1;
	}	
	ptNew->tPixelDatas.aucPixelDatas = g_ptDefaultDispOpr->pucDispMem;
	
	ptNew->iID = 0;
	ptNew->bDevFrameBuffer = 1;
	ptNew->eVideoMemState = VMS_FREE;
	ptNew->ePicState	  = PS_BLANK;
	ptNew->tPixelDatas.iWidth  = iXres;
	ptNew->tPixelDatas.iHeight = iYres;
	ptNew->tPixelDatas.iBpp    = iBpp;
	ptNew->tPixelDatas.iLineBytes  = iLineBytes;
	ptNew->tPixelDatas.iTotalBytes = iVMSize;

	if (iNum != 0)
	{
		ptNew->eVideoMemState = VMS_USED_FOR_CUR;
	}
	
	/* �������� */
	ptNew->ptNext = g_ptVideoMemHead;
	g_ptVideoMemHead = ptNew;
	
	
	for (i = 0; i < iNum; i++)
	{
		ptNew = malloc(sizeof(T_VideoMem) + iVMSize);
		if (ptNew == NULL)
		{
			return -1;
		}
		ptNew->tPixelDatas.aucPixelDatas = (unsigned char *)(ptNew + 1);

		ptNew->iID = 0;
		ptNew->bDevFrameBuffer = 0;
		ptNew->eVideoMemState = VMS_FREE;
		ptNew->ePicState      = PS_BLANK;
		ptNew->tPixelDatas.iWidth  = iXres;
		ptNew->tPixelDatas.iHeight = iYres;
		ptNew->tPixelDatas.iBpp    = iBpp;
		ptNew->tPixelDatas.iLineBytes = iLineBytes;
		ptNew->tPixelDatas.iTotalBytes = iVMSize;

		/* �������� */
		ptNew->ptNext = g_ptVideoMemHead;
		g_ptVideoMemHead = ptNew;
	}
	
	return 0;
}

PT_VideoMem GetVideoMem(int iID, int bCur)
{
	PT_VideoMem ptTmp = g_ptVideoMemHead;
	
	/* 1. ����: ȡ�����еġ�ID��ͬ��videomem */
	while (ptTmp)
	{
		if ((ptTmp->eVideoMemState == VMS_FREE) && (ptTmp>iID == iID))
		{
			ptTmp->eVideoMemState = bCur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	
	/* 2. ����: ȡ������һ������videomem */
	ptTmp = g_ptVideoMemHead;
	while (ptTmp)
	{
		if (ptTmp->eVideoMemState == VMS_FREE)
		{
			ptTmp->iID = iID;
			ptTmp->ePicState = PS_BLANK;
			ptTmp->eVideoMemState = bCur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}

	return NULL;
}

void PutVideoMem(PT_VideoMem ptVideoMem)
{
	ptVideoMem->eVideoMemState = VMS_FREE;
}


int DisplayInit(void)
{
	int iError;
	
	iError = FBInit();

	return iError;
}

