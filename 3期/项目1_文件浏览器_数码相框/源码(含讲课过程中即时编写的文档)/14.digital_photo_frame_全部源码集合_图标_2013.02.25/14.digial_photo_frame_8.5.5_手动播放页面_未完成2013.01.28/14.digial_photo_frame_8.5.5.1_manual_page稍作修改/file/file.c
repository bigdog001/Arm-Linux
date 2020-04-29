
#include <config.h>
#include <file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


int MapFile(PT_FileMap ptFileMap)
{
	int iFd;
    FILE *tFp;
	struct stat tStat;
	
	/* 打开文件 */
	tFp = fopen(ptFileMap->strFileName, "r+");
	if (tFp == NULL)
	{
		DBG_PRINTF("can't open %s\n", ptFileMap->strFileName);
		return -1;
	}
	ptFileMap->tFp = tFp;
    iFd = fileno(tFp);

	fstat(iFd, &tStat);
	ptFileMap->iFileSize = tStat.st_size;
	ptFileMap->pucFileMapMem = (unsigned char *)mmap(NULL , tStat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, iFd, 0);
	if (ptFileMap->pucFileMapMem == (unsigned char *)-1)
	{
		DBG_PRINTF("mmap error!\n");
		return -1;
	}
	return 0;
}

void UnMapFile(PT_FileMap ptFileMap)
{
	munmap(ptFileMap->pucFileMapMem, ptFileMap->iFileSize);
	fclose(ptFileMap->tFp);
}

static int isDir(char *strFilePath, char *strFileName)
{
    char strTmp[256];
    struct stat tStat;

    snprintf(strTmp, 256, "%s/%s", strFilePath, strFileName);
    strTmp[255] = '\0';

    if ((stat(strTmp, &tStat) == 0) && S_ISDIR(tStat.st_mode))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int isRegDir(char *strDirPath, char *strSubDirName)
{
    static const char *astrSpecailDirs[] = {"sbin", "bin", "usr", "lib", "proc", "tmp", "dev", "sys", NULL};
    int i;
    
    /* 如果目录名含有"astrSpecailDirs"中的任意一个, 则返回0 */
    if (0 == strcmp(strDirPath, "/"))
    {
        while (astrSpecailDirs[i])
        {
            if (0 == strcmp(strSubDirName, astrSpecailDirs[i]))
                return 0;
            i++;
        }
    }
    return 1;    
}

static int isRegFile(char *strFilePath, char *strFileName)
{
    char strTmp[256];
    struct stat tStat;

    snprintf(strTmp, 256, "%s/%s", strFilePath, strFileName);
    strTmp[255] = '\0';

    if ((stat(strTmp, &tStat) == 0) && S_ISREG(tStat.st_mode))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}



/* 把某目录下所含的顶层子目录、顶层目录下的文件都记录下来 */
int GetDirContents(char *strDirName, PT_DirContent **pptDirContents, int *piNumber)	
{
    PT_DirContent *aptDirContents;
	struct dirent **aptNameList;
	int iNumber;
	int i;
	int j;

	/* 扫描目录,结果按名字排序,存在aptNameList[0],aptNameList[1],... */
	iNumber = scandir(strDirName, &aptNameList, 0, alphasort);
	if (iNumber < 0)
	{
		DBG_PRINTF("scandir error : %s!\n", strDirName);
		return -1;
	}

	/* 忽略".", ".."这两个目录 */
	aptDirContents = malloc(sizeof(PT_DirContent) * (iNumber - 2));
	if (NULL == aptDirContents)
	{
		DBG_PRINTF("malloc error!\n");
		return -1;
	}
    *pptDirContents = aptDirContents;

	for (i = 0; i < iNumber - 2; i++)
	{
		aptDirContents[i] = malloc(sizeof(T_DirContent));
		if (NULL == aptDirContents)
		{
			DBG_PRINTF("malloc error!\n");
			return -1;
		}
	}

	/* 先把目录挑出来存入aptDirContents */
	for (i = 0, j = 0; i < iNumber; i++)
	{
		/* 忽略".",".."这两个目录 */
		if ((0 == strcmp(aptNameList[i]->d_name, ".")) || (0 == strcmp(aptNameList[i]->d_name, "..")))
			continue;
        /* 并不是所有的文件系统都支持d_type, 所以不能直接判断d_type */
		/* if (aptNameList[i]->d_type == DT_DIR) */
        if (isDir(strDirName, aptNameList[i]->d_name))
		{
			strncpy(aptDirContents[j]->strName, aptNameList[i]->d_name, 256);
			aptDirContents[j]->strName[255] = '\0';
			aptDirContents[j]->eFileType    = FILETYPE_DIR;
            free(aptNameList[i]);
            aptNameList[i] = NULL;
			j++;
		}
	}

	/* 再把常规文件挑出来存入aptDirContents */
	for (i = 0; i < iNumber; i++)
	{
        if (aptNameList[i] == NULL)
            continue;
        
		/* 忽略".",".."这两个目录 */
		if ((0 == strcmp(aptNameList[i]->d_name, ".")) || (0 == strcmp(aptNameList[i]->d_name, "..")))
			continue;
        /* 并不是所有的文件系统都支持d_type, 所以不能直接判断d_type */
		/* if (aptNameList[i]->d_type == DT_REG) */
        if (isRegFile(strDirName, aptNameList[i]->d_name))
		{
			strncpy(aptDirContents[j]->strName, aptNameList[i]->d_name, 256);
			aptDirContents[j]->strName[255] = '\0';
			aptDirContents[j]->eFileType    = FILETYPE_FILE;
            free(aptNameList[i]);
            aptNameList[i] = NULL;
			j++;
		}
	}

	/* 释放aptDirContents中未使用的项 */
	for (i = j; i < iNumber - 2; i++)
	{
		free(aptDirContents[i]);
	}

	/* 释放scandir函数分配的内存 */
	for (i = 0; i < iNumber; i++)
	{
        if (aptNameList[i])
        {
    		free(aptNameList[i]);
        }
	}
	free(aptNameList);

	*piNumber = j;
	
	return 0;
}

void FreeDirContents(PT_DirContent *aptDirContents, int iNumber)
{
	int i;
	for (i = 0; i < iNumber; i++)
	{
		free(aptDirContents[i]);
	}
	free(aptDirContents);
}


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
 * piStartNumberToRecord : 从第几个文件开始取出它们的名字
 * piCurFileNumber       : 本次函数执行时读到的第1个文件的编号
 * piFileCountHaveGet    : 已经得到了多少个文件的名字
 * iFileCountTotal       : 总共要取出多少个文件的名字
 *
 */
int GetFilesIndir(char *strDirName, int *piStartNumberToRecord, int *piCurFileNumber, int *piFileCountHaveGet, int iFileCountTotal, char apstrFileNames[][256])
{
    int iError;
    PT_DirContent *aptDirContents;  /* 数组:存有目录下"顶层子目录","文件"的名字 */
    int iDirContentsNumber;         /* aptDirContents数组有多少项 */
    int i;
    char strSubDirName[256];

    /* 为避免访问的目录互相嵌套, 设置能访问的目录深度为10 */
#define MAX_DIR_DEEPNESS 10    
    static int iDirDeepness = 0;

    if (iDirDeepness > MAX_DIR_DEEPNESS)
    {
        return -1;
    }

    iDirDeepness++;    
    
    iError = GetDirContents(strDirName, &aptDirContents, &iDirContentsNumber);    
    if (iError)
    {
        DBG_PRINTF("GetDirContents error!\n");
        iDirDeepness--;
        return -1;
    }

    /* 先记录文件 */
    for (i = 0; i < iDirContentsNumber; i++)
    {
        if (aptDirContents[i]->eFileType == FILETYPE_FILE)
        {
            if (*piCurFileNumber >= *piStartNumberToRecord)
            {
                snprintf(apstrFileNames[*piFileCountHaveGet], 256, "%s/%s", strDirName, aptDirContents[i]->strName);
                (*piFileCountHaveGet)++;
                (*piCurFileNumber)++;
                (*piStartNumberToRecord)++;
                if (*piFileCountHaveGet >= iFileCountTotal)
                {
                    FreeDirContents(aptDirContents, iDirContentsNumber);
                    iDirDeepness--;
                    return 0;
                }
            }
            else
            {
                (*piCurFileNumber)++;
            }
        }
    }

    /* 递归处理目录 */
    for (i = 0; i < iDirContentsNumber; i++)
    {
        if ((aptDirContents[i]->eFileType == FILETYPE_DIR) && isRegDir(strDirName, aptDirContents[i]->strName))
        {
            snprintf(strSubDirName, 256, "%s/%s", strDirName, aptDirContents[i]->strName);
            GetFilesIndir(strSubDirName, piStartNumberToRecord, piCurFileNumber, piFileCountHaveGet, iFileCountTotal, apstrFileNames);
            if (*piFileCountHaveGet >= iFileCountTotal)
            {
                FreeDirContents(aptDirContents, iDirContentsNumber);
                iDirDeepness--;
                return 0;
            }
        }
    }

    FreeDirContents(aptDirContents, iDirContentsNumber);
    iDirDeepness--;
    return 0;
}


