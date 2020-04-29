
void touchscreen_test(void)
{
	unsigned int fb_base;
	int xres, yres, bpp;

	int x, y;

	/* ���LCD�Ĳ���: fb_base, xres, yres, bpp */
	get_lcd_params(&fb_base, &xres, &yres, &bpp);

	touchscreen_init();

	/* ���� */
	clear_screen(0);

	/* ��ʾ������ʾ��׼ */
	fb_print_string(70, 70, "Touc cross to calibrate touchscreen", 0xffffff);
	ts_calibrate();

	/* ��ʾ������ʾ�滭 */
	fb_print_string(70, yres - 70, "OK! To draw!", 0xffffff);

	while (1)
	{
		ts_read(&x, &y);
		printf(" x = %d, y = %d\n\r", x, y);
		fb_put_pixel(x, y, 0xff00);
	}
}

