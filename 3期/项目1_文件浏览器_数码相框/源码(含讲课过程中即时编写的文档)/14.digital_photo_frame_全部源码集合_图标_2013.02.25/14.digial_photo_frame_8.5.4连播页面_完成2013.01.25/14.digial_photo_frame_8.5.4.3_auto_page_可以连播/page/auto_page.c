#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <file.h>
#include <string.h>
#include <unistd.h>

static pthread_t g_tAutoPlayThreadID;
static pthread_mutex_t g_tAutoPlayThreadMutex  = PTHREAD_MUTEX_INITIALIZER; /* ������ */
static int g_bAutoPlayThreadShouldExit = 0;
static T_PageCfg g_tPageCfg;

/* ��������ȵķ�ʽ���Ŀ¼�µ��ļ� 
 * ��: �Ȼ�ö���Ŀ¼�µ��ļ�, �ٽ���һ����Ŀ¼A
 *     �Ȼ��һ����Ŀ¼A�µ��ļ�, �ٽ��������Ŀ¼AA, ...
 *     ������һ����Ŀ¼A��, �ٽ���һ����Ŀ¼B
 *
 * "����ģʽ"�µ��øú������Ҫ��ʾ���ļ�
 * �����ַ��������Щ�ļ�:
 * 1. ����ֻ��Ҫ����һ�κ���,�������ļ������ֱ��浽ĳ����������
 * 2. Ҫʹ���ļ�ʱ�ٵ��ú���,ֻ���浱ǰҪʹ�õ��ļ�������
 * ��1�ַ����Ƚϼ�,���ǵ��ļ��ܶ�ʱ�п��ܵ����ڴ治��.
 * ����ʹ�õ�2�ַ���:
 * ����ĳĿ¼(����������Ŀ¼)�����е��ļ���������һ����
 * g_iStartNumberToRecord : �ӵڼ����ļ���ʼȡ�����ǵ�����
 * g_iCurFileNumber       : ���κ���ִ��ʱ�����ĵ�1���ļ��ı��
 * g_iFileCountHaveGet    : �Ѿ��õ��˶��ٸ��ļ�������
 * g_iFileCountTotal      : ÿһ���ܹ�Ҫȡ�����ٸ��ļ�������
 * g_iNextProcessFileIndex: ��g_apstrFileNames�����м���Ҫ��ʾ��LCD�ϵ��ļ�
 *
 */
static int g_iStartNumberToRecord = 0;
static int g_iCurFileNumber = 0;
static int g_iFileCountHaveGet = 0;
static int g_iFileCountTotal = 0;
static int g_iNextProcessFileIndex = 0;

#define FILE_COUNT 10
static char g_apstrFileNames[FILE_COUNT][256];

static void ResetAutoPlayFile(void)
{
    g_iStartNumberToRecord = 0;
    g_iCurFileNumber = 0;
    g_iFileCountHaveGet = 0;
    g_iFileCountTotal = 0;
    g_iNextProcessFileIndex = 0;
}

static int GetNextAutoPlayFile(char *strFileName)
{
    int iError;
    
    if (g_iNextProcessFileIndex < g_iFileCountHaveGet)
    {
        strncpy(strFileName, g_apstrFileNames[g_iNextProcessFileIndex], 256);
        g_iNextProcessFileIndex++;
        return 0;
    }
    else
    {
        g_iCurFileNumber    = 0;
        g_iFileCountHaveGet = 0;
        g_iFileCountTotal   = FILE_COUNT;
        g_iNextProcessFileIndex = 0;
        iError = GetFilesIndir(g_tPageCfg.strSeletedDir, &g_iStartNumberToRecord, &g_iCurFileNumber, &g_iFileCountHaveGet, g_iFileCountTotal, g_apstrFileNames);
        if (iError || (g_iNextProcessFileIndex >= g_iFileCountHaveGet))
        {
            /* �ٴδ�ͷ����(����ģʽ��ѭ����ʾ) */
            g_iStartNumberToRecord = 0;
            g_iCurFileNumber    = 0;
            g_iFileCountHaveGet = 0;
            g_iFileCountTotal = FILE_COUNT;
            g_iNextProcessFileIndex = 0;
            
            iError = GetFilesIndir(g_tPageCfg.strSeletedDir, &g_iStartNumberToRecord, &g_iCurFileNumber, &g_iFileCountHaveGet, g_iFileCountTotal, g_apstrFileNames);
        }
        
        if (iError == 0)
        {   
            if (g_iNextProcessFileIndex < g_iFileCountHaveGet)
            {
                strncpy(strFileName, g_apstrFileNames[g_iNextProcessFileIndex], 256);
                g_iNextProcessFileIndex++;
                return 0;
            }
        }
    }

    return -1;
}

/* bCur = 1 ��ʾ������videomem, ��Ϊ�������Ͼ�Ҫ��LCD����ʾ������
 * bCur = 0 ��ʾ������׼���õ�, �п����޷����videomem
 */
static PT_VideoMem PrepareNextPicture(int bCur)
{
	T_PixelDatas tOriginIconPixelDatas;
	T_PixelDatas tPicPixelDatas;
    PT_VideoMem ptVideoMem;
	int iError;
	int iXres, iYres, iBpp;
    int iTopLeftX, iTopLeftY;
    float k;
    char strFileName[256];
    
	GetDispResolution(&iXres, &iYres, &iBpp);
    
	/* ����Դ� */
	ptVideoMem = GetVideoMem(-1, bCur);
	if (ptVideoMem == NULL)
	{
		DBG_PRINTF("can't get video mem for browse page!\n");
		return NULL;
	}
    ClearVideoMem(ptVideoMem, COLOR_BACKGROUND);

    while (1)
    {
        iError = GetNextAutoPlayFile(strFileName);
        if (iError)
        {
            DBG_PRINTF("GetNextAutoPlayFile error\n");
            PutVideoMem(ptVideoMem);
            return NULL;
        }
        
        iError = GetPixelDatasFrmFile(strFileName, &tOriginIconPixelDatas);
        if (0 == iError)
        {
            break;
        }
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
        PutVideoMem(ptVideoMem);
        return NULL;
    }

    PicZoom(&tOriginIconPixelDatas, &tPicPixelDatas);

    /* ���������ʾʱ���Ͻ����� */
    iTopLeftX = (iXres - tPicPixelDatas.iWidth) / 2;
    iTopLeftY = (iYres - tPicPixelDatas.iHeight) / 2;
    PicMerge(iTopLeftX, iTopLeftY, &tPicPixelDatas, &ptVideoMem->tPixelDatas);
    FreePixelDatasFrmFile(&tOriginIconPixelDatas);
    free(tPicPixelDatas.aucPixelDatas);

    return ptVideoMem;
}

static void *AutoPlayThreadFunction(void *pVoid)
{
    int bExit;
    PT_VideoMem ptVideoMem;

    ResetAutoPlayFile();
    
    while (1)
    {
        /* 1. ���ж��Ƿ�Ҫ�˳� */
        pthread_mutex_lock(&g_tAutoPlayThreadMutex);
        bExit = g_bAutoPlayThreadShouldExit;
        pthread_mutex_unlock(&g_tAutoPlayThreadMutex);

        if (bExit)
        {
            return NULL;
        }

        /* 2. ׼��Ҫ��ʾ��ͼƬ */
        ptVideoMem = PrepareNextPicture(0);

        /* 3. ʱ�䵽�����ʾ���� */
        if (ptVideoMem == NULL)
        {
            ptVideoMem = PrepareNextPicture(1);
        }

    	/* ˢ���豸��ȥ */
    	FlushVideoMemToDev(ptVideoMem);

    	/* ����Դ� */
    	PutVideoMem(ptVideoMem);        

        sleep(g_tPageCfg.iIntervalSecond);       /* �������������� */
    }
    return NULL;
}

static void AutoPageRun(void)
{
	T_InputEvent tInputEvent;
	int iRet;
    
    g_bAutoPlayThreadShouldExit = 0;

    /* �������ֵ: ��ʾ��һ��Ŀ¼, ��ʾͼƬ�ļ�� */
    GetPageCfg(&g_tPageCfg);
        
    /* 1. ����һ���߳���������ʾͼƬ */
    pthread_create(&g_tAutoPlayThreadID, NULL, AutoPlayThreadFunction, NULL);

    /* 2. ��ǰ�̵߳ȴ�����������, �����򵥵�: �������˴�����, ���߳��˳� */
    while (1)
    {
        iRet = GetInputEvent(&tInputEvent);
        if (0 == iRet)
        {
            pthread_mutex_lock(&g_tAutoPlayThreadMutex);
            g_bAutoPlayThreadShouldExit = 1;   /* AutoPlayThreadFunction�̼߳�⵽�������Ϊ1����˳� */
            pthread_mutex_unlock(&g_tAutoPlayThreadMutex);
            sleep(2); /* Ҳ��AutoPlayThreadFunction�߳��˳�û��ô��,�������Ե�һ�� */
            return;
        }
    }

}

static T_PageAction g_tAutoPageAction = {
	.name          = "auto",
	.Run           = AutoPageRun,
};

int AutoPageInit(void)
{
	return RegisterPageAction(&g_tAutoPageAction);
}


