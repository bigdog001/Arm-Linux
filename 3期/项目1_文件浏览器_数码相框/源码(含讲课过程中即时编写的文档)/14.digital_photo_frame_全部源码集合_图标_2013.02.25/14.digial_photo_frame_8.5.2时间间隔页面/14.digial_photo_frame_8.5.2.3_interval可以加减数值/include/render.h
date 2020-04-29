
#ifndef _RENDER_H
#define _RENDER_H

#include <pic_operation.h>
#include <disp_manager.h>
#include <page_manager.h>

int PicZoom(PT_PixelDatas ptOriginPic, PT_PixelDatas ptZoomPic);
int PicMerge(int iX, int iY, PT_PixelDatas ptSmallPic, PT_PixelDatas ptBigPic);
void FlushVideoMemToDev(PT_VideoMem ptVideoMem);
int GetPixelDatasForIcon(char *strFileName, PT_PixelDatas ptPixelDatas);
void FreePixelDatasForIcon(PT_PixelDatas ptPixelDatas);
void ReleaseButton(PT_Layout ptLayout);
void PressButton(PT_Layout ptLayout);
int MergerStringToCenterOfRectangleInVideoMem(int iTopLeftX, int iTopLeftY, int iBotRightX, int iBotRightY, unsigned char *pucTextString, PT_VideoMem ptVideoMem);


#endif /* _RENDER_H */

