#ifndef _DISP_MANAGER_H
#define _DISP_MANAGER_H

typedef struct DispOpr {
	char *name;
	int iXres;
	int iYres;
	int iBpp;
	unsigned char *pucDispMem;
	int (*DeviceInit)(void);
	int (*ShowPixel)(int iPenX, int iPenY, unsigned int dwColor);
	int (*CleanScreen)(unsigned int dwBackColor);
	struct DispOpr *ptNext;
}T_DispOpr, *PT_DispOpr;

int RegisterDispOpr(PT_DispOpr ptDispOpr);
void ShowDispOpr(void);
int DisplayInit(void);
void SelectDefaultDispDev(char *name);
int GetDispResolution(int *piXres, int *piYres);

int FBInit(void);

#endif /* _DISP_MANAGER_H */

