

/* 向上: 接收不同LCD的参数
 * 向下: 使用这些参数设置对应的LCD控制器
 */

void lcd_controller_init(p_lcd_params plcdparams)
{
	/* 调用2440的LCD控制器的初始化函数 */
	lcd_controller.init(plcdparams);
}

