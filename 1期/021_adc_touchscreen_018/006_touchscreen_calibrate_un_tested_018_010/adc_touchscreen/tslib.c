
static double g_kx;
static double g_ky;

static int g_ts_xc, g_ts_yc;
static int g_lcd_xc, g_lcd_yc;
static int g_ts_xy_swap = 0;

void get_calibrate_point_data(int lcd_x, int lcd_y, int *px, int *py)
{
	fb_disp_cross(lcd_x, lcd_y, 0xffffff);

	/* 等待点击 */

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
		return 0; /* xy没有反转 */
	else
		return 1; /* xy反了 */
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

	/* X轴方向 */
	int ts_s1, ts_s2;
	int lcd_s;

	/* Y轴方向 */
	int ts_d1, ts_d2;
	int lcd_d;

	/* 获得LCD的参数: fb_base, xres, yres, bpp */
	get_lcd_params(&fb_base, &xres, &yres, &bpp);

	/* 对于ABCDE, 循环: 显示"+"、点击、读ts原始值 */
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

	/* 确定触摸屏数据XY是否反转 */
	g_ts_xy_swap = is_ts_xy_swap(a_ts_x, a_ts_y, b_ts_x, b_ts_y);

	if (g_ts_xy_swap)
	{
		/* 对调所有点的XY坐标 */
		swap_xy(&a_ts_x, &a_ts_y);
		swap_xy(&b_ts_x, &b_ts_y);
		swap_xy(&c_ts_x, &c_ts_y);
		swap_xy(&d_ts_x, &d_ts_y);
		swap_xy(&e_ts_x, &e_ts_y);
	}

	/* 确定公式的参数并保存 */
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
 * 读TS原始数据, 转换为LCD坐标
 */
void ts_read(int *lcd_x, int *lcd_y)
{
	int ts_x, ts_y;
	
	ts_read_raw(&ts_x, ts_y);

	if (g_ts_xy_swap)
	{
		swap_xy(&ts_x, &ts_y);
	}

	/* 使用公式计算 */
	*lcd_x = g_kx * (ts_x - g_ts_xc) + g_lcd_xc;
	*lcd_y = g_ky * (ts_y - g_ts_yc) + g_lcd_yc;
}

