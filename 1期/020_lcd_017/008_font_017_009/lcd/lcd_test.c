
#include "geometry.h"
#include "font.h"

void lcd_test(void)
{
	unsigned int fb_base;
	int xres, yres, bpp;
	int x, y;
	unsigned short *p;
	unsigned int *p2;
		
	/* ��ʼ��LCD */
	lcd_init();

	/* ʹ��LCD */
	lcd_enable();

	/* ���LCD�Ĳ���: fb_base, xres, yres, bpp */
	get_lcd_params(&fb_base, &xres, &yres, &bpp);
	fb_get_lcd_params();
	font_init();
	
	/* ��framebuffer��д���� */
	if (bpp == 16)
	{
		/* ��LCD��������ĺ�ɫ */

		/* 565: 0xf800 */

		p = (unsigned short *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p++ = 0xf800;

		/* green */
		p = (unsigned short *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p++ = 0x7e0;

		/* blue */
		p = (unsigned short *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p++ = 0x1f;

		/* black */
		p = (unsigned short *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p++ = 0;
			
	}
	else if (bpp == 32)
	{
		/* ��LCD��������ĺ�ɫ */

		/* 0xRRGGBB */

		p2 = (unsigned int *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p2++ = 0xff0000;

		/* green */
		p2 = (unsigned int *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p2++ = 0x00ff00;

		/* blue */
		p2 = (unsigned int *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p2++ = 0x0000ff;

		/* black */
		p2 = (unsigned int *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p2++ = 0;

	}

	delay(1000000);
	
	/* ���� */
	draw_line(0, 0, xres - 1, 0, 0xff0000);
	draw_line(xres - 1, 0, xres - 1, yres - 1, 0xffff00);
	draw_line(0, yres - 1, xres - 1, yres - 1, 0xff00aa);
	draw_line(0, 0, 0, yres - 1, 0xff00ef);
	draw_line(0, 0, xres - 1, yres - 1, 0xff4500);
	draw_line(xres - 1, 0, 0, yres - 1, 0xff0780);

	delay(1000000);

	/* ��Բ */
	draw_circle(xres/2, yres/2, yres/4, 0xff00);

	/* ������� */
	fb_print_string(10, 10, "www.100ask.net\n\r100ask.taobao.com", 0xff00);
}

