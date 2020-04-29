#include <config.h>
#include <render.h>
#include <stdlib.h>
#include <file.h>
#include <string.h>
#include <unistd.h>

static pthread_t g_tAutoPlayThreadID;
static pthread_mutex_t g_tAutoPlayThreadMutex  = PTHREAD_MUTEX_INITIALIZER; /* 互斥量 */
static int g_bAutoPlayThreadShouldExit = 0;
static T_PageCfg g_tPageCfg;

/* 以深度优先的方式获得目录下的文件 
 * 即: 先获得顶层目录下的文件, 再进入一级子目录A
 *     先获得一级子目录A下的文件, 再进入二级子目录AA, ...
 *     处理完一级子目录A后, 再进入一级子目录B
 *
 * "连播模式"下调用该函数获得要显示的文件
 * 有两种方法获得这些文件:
 * 1. 事先只需要调用一次函数,把所有文件的名字保存到某个缓冲区中
 * 2. 要使用文件时再调用函数,只保存当前要使用的文件的名字
 * 第1种方法比较简单,但是当文件很多时有可能导致内存不足.
 * 我们使用第2种方法:
 * 假设某目录(包括所有子目录)下所有的文件都给它编一个号
 * g_iStartNumberToRecord : 从第几个文件开始取出它们的名字
 * g_iCurFileNumber       : 本次函数执行时读到的第1个文件的编号
 * g_iFileCountHaveGet    : 已经得到了多少个文件的名字
 * g_iFileCountTotal      : 每一次总共要取出多少个文件的名字
 * g_iNextProcessFileIndex: 在g_apstrFileNames数组中即将要显示在LCD上的文件
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
            /* 再次从头读起(连播模式下循环显示) */
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

/* bCur = 1 表示必须获得videomem, 因为这是马上就要在LCD上显示出来的
 * bCur = 0 表示这是做准备用的, 有可能无法获得videomem
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
    
	/* 获得显存 */
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
        PutVideoMem(ptVideoMem);
        return NULL;
    }

    PicZoom(&tOriginIconPixelDatas, &tPicPixelDatas);

    /* 算出居中显示时左上角坐标 */
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
        /* 1. 先判断是否要退出 */
        pthread_mutex_lock(&g_tAutoPlayThreadMutex);
        bExit = g_bAutoPlayThreadShouldExit;
        pthread_mutex_unlock(&g_tAutoPlayThreadMutex);

        if (bExit)
        {
            return NULL;
        }

        /* 2. 准备要显示的图片 */
        ptVideoMem = PrepareNextPicture(0);

        /* 3. 时间到后就显示出来 */
        if (ptVideoMem == NULL)
        {
            ptVideoMem = PrepareNextPicture(1);
        }

    	/* 刷到设备上去 */
    	FlushVideoMemToDev(ptVideoMem);

    	/* 解放显存 */
    	PutVideoMem(ptVideoMem);        

        sleep(g_tPageCfg.iIntervalSecond);       /* 先用休眠来代替 */
    }
    return NULL;
}

static void AutoPageRun(void)
{
	T_InputEvent tInputEvent;
	int iRet;
    
    g_bAutoPlayThreadShouldExit = 0;

    /* 获得配置值: 显示哪一个目录, 显示图片的间隔 */
    GetPageCfg(&g_tPageCfg);
        
    /* 1. 启动一个线程来连续显示图片 */
    pthread_create(&g_tAutoPlayThreadID, NULL, AutoPlayThreadFunction, NULL);

    /* 2. 当前线程等待触摸屏输入, 先做简单点: 如果点击了触摸屏, 让线程退出 */
    while (1)
    {
        iRet = GetInputEvent(&tInputEvent);
        if (0 == iRet)
        {
            pthread_mutex_lock(&g_tAutoPlayThreadMutex);
            g_bAutoPlayThreadShouldExit = 1;   /* AutoPlayThreadFunction线程检测到这个变量为1后会退出 */
            pthread_mutex_unlock(&g_tAutoPlayThreadMutex);
            sleep(2); /* 也许AutoPlayThreadFunction线程退出没那么快,在这里稍等一会 */
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


