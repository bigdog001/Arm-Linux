#include <stdio.h>
#include "jpeglib.h"
#include <setjmp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <string.h>
#include <stdlib.h>

#define FB_DEVICE_NAME "/dev/fb0"
#define DBG_PRINTF printf

static int g_fd;

static struct fb_var_screeninfo g_tFBVar;
static struct fb_fix_screeninfo g_tFBFix;			
static unsigned char *g_pucFBMem;
static unsigned int g_dwScreenSize;

static unsigned int g_dwLineWidth;
static unsigned int g_dwPixelWidth;

static int FBDeviceInit(void)
{
	int ret;
	
	g_fd = open(FB_DEVICE_NAME, O_RDWR);
	if (0 > g_fd)
	{
		DBG_PRINTF("can't open %s\n", FB_DEVICE_NAME);
	}

	ret = ioctl(g_fd, FBIOGET_VSCREENINFO, &g_tFBVar);
	if (ret < 0)
	{
		DBG_PRINTF("can't get fb's var\n");
		return -1;
	}

	ret = ioctl(g_fd, FBIOGET_FSCREENINFO, &g_tFBFix);
	if (ret < 0)
	{
		DBG_PRINTF("can't get fb's fix\n");
		return -1;
	}
	
	g_dwScreenSize = g_tFBVar.xres * g_tFBVar.yres * g_tFBVar.bits_per_pixel / 8;
	g_pucFBMem = (unsigned char *)mmap(NULL , g_dwScreenSize, PROT_READ | PROT_WRITE, MAP_SHARED, g_fd, 0);
	if (0 > g_pucFBMem)	
	{
		DBG_PRINTF("can't mmap\n");
		return -1;
	}

	g_dwLineWidth  = g_tFBVar.xres * g_tFBVar.bits_per_pixel / 8;
	g_dwPixelWidth = g_tFBVar.bits_per_pixel / 8;
	
	return 0;
}


static int FBShowPixel(int iX, int iY, unsigned int dwColor)
{
	unsigned char *pucFB;
	unsigned short *pwFB16bpp;
	unsigned int *pdwFB32bpp;
	unsigned short wColor16bpp; /* 565 */
	int iRed;
	int iGreen;
	int iBlue;

	if ((iX >= g_tFBVar.xres) || (iY >= g_tFBVar.yres))
	{
		DBG_PRINTF("out of region\n");
		return -1;
	}

	pucFB      = g_pucFBMem + g_dwLineWidth * iY + g_dwPixelWidth * iX;
	pwFB16bpp  = (unsigned short *)pucFB;
	pdwFB32bpp = (unsigned int *)pucFB;
	
	switch (g_tFBVar.bits_per_pixel)
	{
		case 8:
		{
			*pucFB = (unsigned char)dwColor;
			break;
		}
		case 16:
		{
			iRed   = (dwColor >> (16+3)) & 0x1f;
			iGreen = (dwColor >> (8+2)) & 0x3f;
			iBlue  = (dwColor >> 3) & 0x1f;
			wColor16bpp = (iRed << 11) | (iGreen << 5) | iBlue;
			*pwFB16bpp	= wColor16bpp;
			break;
		}
		case 32:
		{
			*pdwFB32bpp = dwColor;
			break;
		}
		default :
		{
			DBG_PRINTF("can't support %d bpp\n", g_tFBVar.bits_per_pixel);
			return -1;
		}
	}

	return 0;
}

static int FBCleanScreen(unsigned int dwBackColor)
{
	unsigned char *pucFB;
	unsigned short *pwFB16bpp;
	unsigned int *pdwFB32bpp;
	unsigned short wColor16bpp; /* 565 */
	int iRed;
	int iGreen;
	int iBlue;
	int i = 0;

	pucFB      = g_pucFBMem;
	pwFB16bpp  = (unsigned short *)pucFB;
	pdwFB32bpp = (unsigned int *)pucFB;

	switch (g_tFBVar.bits_per_pixel)
	{
		case 8:
		{
			memset(g_pucFBMem, dwBackColor, g_dwScreenSize);
			break;
		}
		case 16:
		{
			iRed   = (dwBackColor >> (16+3)) & 0x1f;
			iGreen = (dwBackColor >> (8+2)) & 0x3f;
			iBlue  = (dwBackColor >> 3) & 0x1f;
			wColor16bpp = (iRed << 11) | (iGreen << 5) | iBlue;
			while (i < g_dwScreenSize)
			{
				*pwFB16bpp	= wColor16bpp;
				pwFB16bpp++;
				i += 2;
			}
			break;
		}
		case 32:
		{
			while (i < g_dwScreenSize)
			{
				*pdwFB32bpp	= dwBackColor;
				pdwFB32bpp++;
				i += 4;
			}
			break;
		}
		default :
		{
			DBG_PRINTF("can't support %d bpp\n", g_tFBVar.bits_per_pixel);
			return -1;
		}
	}

	return 0;
}

static int FBShowLine(int iXStart, int iXEnd, int iY, unsigned char *pucRGBArray)
{
	int i = iXStart * 3;
	int iX;
	unsigned int dwColor;

	if (iY >= g_tFBVar.yres)
		return -1;

	if (iXStart >= g_tFBVar.xres)
		return -1;

	if (iXEnd >= g_tFBVar.xres)
	{
		iXEnd = g_tFBVar.xres;		
	}
	
	for (iX = iXStart; iX < iXEnd; iX++)
	{
		/* 0xRRGGBB */
		dwColor = (pucRGBArray[i]<<16) + (pucRGBArray[i+1]<<8) + (pucRGBArray[i+2]<<0);
		i += 3;
		FBShowPixel(iX, iY, dwColor);
	}
	return 0;
}


/*
Allocate and initialize a JPEG decompression object    // ����ͳ�ʼ��һ��decompression�ṹ��
Specify the source of the compressed data (eg, a file) // ָ��Դ�ļ�
Call jpeg_read_header() to obtain image info		   // ��jpeg_read_header���jpg��Ϣ
Set parameters for decompression					   // ���ý�ѹ����,����Ŵ���С
jpeg_start_decompress(...); 						   // ������ѹ��jpeg_start_decompress
while (scan lines remain to be read)
	jpeg_read_scanlines(...);						   // ѭ������jpeg_read_scanlines
jpeg_finish_decompress(...);						   // jpeg_finish_decompress
Release the JPEG decompression object				   // �ͷ�decompression�ṹ��
*/

/* Uage: jpg2rgb <jpg_file>
 */

int main(int argc, char **argv)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * infile;
	int row_stride;
	unsigned char *buffer;

	if (argc != 2)
	{
		printf("Usage: \n");
		printf("%s <jpg_file>\n", argv[0]);
		return -1;
	}

	if (FBDeviceInit())
	{
		return -1;
	}

	FBCleanScreen(0);

	// ����ͳ�ʼ��һ��decompression�ṹ��
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	// ָ��Դ�ļ�
	if ((infile = fopen(argv[1], "rb")) == NULL) {
		fprintf(stderr, "can't open %s\n", argv[1]);
		return -1;
	}
	jpeg_stdio_src(&cinfo, infile);

	// ��jpeg_read_header���jpg��Ϣ
	jpeg_read_header(&cinfo, TRUE);
	/* Դ��Ϣ */
	printf("image_width = %d\n", cinfo.image_width);
	printf("image_height = %d\n", cinfo.image_height);
	printf("num_components = %d\n", cinfo.num_components);

	// ���ý�ѹ����,����Ŵ���С
	printf("enter scale M/N:\n");
	scanf("%d/%d", &cinfo.scale_num, &cinfo.scale_denom);
	printf("scale to : %d/%d\n", cinfo.scale_num, cinfo.scale_denom);

	// ������ѹ��jpeg_start_decompress	
	jpeg_start_decompress(&cinfo);

	/* �����ͼ�����Ϣ */
	printf("output_width = %d\n", cinfo.output_width);
	printf("output_height = %d\n", cinfo.output_height);
	printf("output_components = %d\n", cinfo.output_components);

	// һ�е����ݳ���
	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = malloc(row_stride);

	// ѭ������jpeg_read_scanlines��һ��һ�еػ�ý�ѹ������
	while (cinfo.output_scanline < cinfo.output_height) 
	{
		(void) jpeg_read_scanlines(&cinfo, &buffer, 1);

		// д��LCDȥ
		FBShowLine(0, cinfo.output_width, cinfo.output_scanline, buffer);
	}
	
	free(buffer);
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	return 0;
}

