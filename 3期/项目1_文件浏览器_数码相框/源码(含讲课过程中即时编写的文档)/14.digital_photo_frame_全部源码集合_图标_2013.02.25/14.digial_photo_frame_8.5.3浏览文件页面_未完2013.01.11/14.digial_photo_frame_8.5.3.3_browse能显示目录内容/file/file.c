
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
	struct stat tStat;
	
	/* 打开文件 */
	iFd = open(ptFileMap->strFileName, O_RDWR);
	if (iFd == -1)
	{
		DBG_PRINTF("can't open %s\n", ptFileMap->strFileName);
		return -1;
	}
	ptFileMap->iFd = iFd;

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
	close(ptFileMap->iFd);
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
		DBG_PRINTF("scandir error!\n");
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

