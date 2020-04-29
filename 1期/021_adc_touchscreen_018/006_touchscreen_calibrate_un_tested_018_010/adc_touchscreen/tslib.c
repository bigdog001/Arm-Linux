
static double g_kx;
static double g_ky;

static int g_ts_xc, g_ts_yc;
static int g_lcd_xc, g_lcd_yc;
static int g_ts_xy_swap = 0;

void get_calibrate_point_data(int lcd_x, int lcd_y, int *px, int *py)
{
	fb_disp_cross(lcd_x, lcd_y, 0xffffff);

	/* �ȴ���� */

	ts_read_raw(px, py);
}


int is_ts_xy_swap(int a_ts_x, int a_ts_y, int b_ts_x, int b_ts_y)
{
	int dx = b_ts_x - a_ts_x;
	int dy = b_ts_y - a_ts_y;

	if (dx < 0)
		dx = 0 - dx;
	if (dy < 0)
		dy = 0 - dy;

	if(dx > dy)
		return 0; /* xyû�з�ת */
	else
		return 1; /* xy���� */
}

void swap_xy(int *px, int *py)
{
	int tmp = *px;
	*px = *py;
	*py = tmp;
}

/*
----------------------------
|                          |
|  +(A)              (B)+  |
|                          |
|                          |
|                          |
|            +(E)          |
|                          |
|                          |
|                          |
|  +(D)              (C)+  |
|                          |
----------------------------

*/

void ts_calibrate(void)
{
	unsigned int fb_base;
	int xres, yres, bpp;

	int a_ts_x, a_ts_y;
	int b_ts_x, b_ts_y;
	int c_ts_x, c_ts_y;
	int d_ts_x, d_ts_y;
	int e_ts_x, e_ts_y;

	/* X�᷽�� */
	int ts_s1, ts_s2;
	int lcd_s;

	/* Y�᷽�� */
	int ts_d1, ts_d2;
	int lcd_d;

	/* ���LCD�Ĳ���: fb_base, xres, yres, bpp */
	get_lcd_params(&fb_base, &xres, &yres, &bpp);

	/* ����ABCDE, ѭ��: ��ʾ"+"���������tsԭʼֵ */
	/* A(50, 50) */
	get_calibrate_point_data(50, 50, &a_ts_x, &a_ts_y);

	/* B(xres-50, 50) */
	get_calibrate_point_data(xres-50, 50, &b_ts_x, &b_ts_y);

	/* C(xres-50, yres-50) */
	get_calibrate_point_data(xres-50, yres-50, &c_ts_x, &c_ts_y);

	/* D(50, yres-50) */
	get_calibrate_point_data(50, yres-50, &d_ts_x, &d_ts_y);
	
	/* E(xres/2, yres/2) */
	get_calibrate_point_data(xres/2, yres/2, &e_ts_x, &e_ts_y);

	/* ȷ������������XY�Ƿ�ת */
	g_ts_xy_swap = is_ts_xy_swap(a_ts_x, a_ts_y, b_ts_x, b_ts_y);

	if (g_ts_xy_swap)
	{
		/* �Ե����е��XY���� */
		swap_xy(&a_ts_x, &a_ts_y);
		swap_xy(&b_ts_x, &b_ts_y);
		swap_xy(&c_ts_x, &c_ts_y);
		swap_xy(&d_ts_x, &d_ts_y);
		swap_xy(&e_ts_x, &e_ts_y);
	}

	/* ȷ����ʽ�Ĳ��������� */
	ts_s1 = b_ts_x - a_ts_x;
	ts_s2 = c_ts_x - d_ts_x;
	lcd_s = xres-50 - 50;

	ts_d1 = d_ts_y - a_ts_y;
	ts_d2 = c_ts_y - b_ts_y;
	lcd_d = yres-50-50;

	g_kx = ((double)(2*lcd_s)) / (ts_s1 + ts_s2);
	g_ky = ((double)(2*lcd_d)) / (ts_d1 + ts_d2);

	g_ts_xc = e_ts_x;
	g_ts_yc = e_ts_y;

	g_lcd_xc = xres/2;
	g_lcd_yc = yres/2;
}

/*
 * ��TSԭʼ����, ת��ΪLCD����
 */
void ts_read(int *lcd_x, int *lcd_y)
{
	int ts_x, ts_y;
	
	ts_read_raw(&ts_x, ts_y);

	if (g_ts_xy_swap)
	{
		swap_xy(&ts_x, &ts_y);
	}

	/* ʹ�ù�ʽ���� */
	*lcd_x = g_kx * (ts_x - g_ts_xc) + g_lcd_xc;
	*lcd_y = g_ky * (ts_y - g_ts_yc) + g_lcd_yc;
}

