#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/suspend.h>
#include <linux/pm_runtime.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>

#include <asm/mach/map.h>
//#include <asm/arch/regs-lcd.h>
//#include <asm/arch/regs-gpio.h>
//#include <asm/arch/fb.h>
static struct platform_device lcd_dev;

static int s3c_lcdfb_setcolreg(unsigned int regno, unsigned int red,
			     unsigned int green, unsigned int blue,
			     unsigned int transp, struct fb_info *info);


struct lcd_regs {
	unsigned long	lcdcon1;
	unsigned long	lcdcon2;
	unsigned long	lcdcon3;
	unsigned long	lcdcon4;
	unsigned long	lcdcon5;
    unsigned long	lcdsaddr1;
    unsigned long	lcdsaddr2;
    unsigned long	lcdsaddr3;
    unsigned long	redlut;
    unsigned long	greenlut;
    unsigned long	bluelut;
    unsigned long	reserved[9];
    unsigned long	dithmode;
    unsigned long	tpal;
    unsigned long	lcdintpnd;
    unsigned long	lcdsrcpnd;
    unsigned long	lcdintmsk;
    unsigned long	lpcsel;
};

static int mylcd_open(struct fb_info *info, int user)
{
	//pm_runtime_get_sync(&lcd_dev.dev);
	pm_runtime_forbid(&lcd_dev.dev);
	return 0;
}
static int mylcd_release(struct fb_info *info, int user)
{
	pm_runtime_mark_last_busy(&lcd_dev.dev);
	//pm_runtime_put_sync_autosuspend(&lcd_dev.dev);
	pm_runtime_allow(&lcd_dev.dev);
	return 0;
}

static struct fb_ops s3c_lcdfb_ops = {
	.owner		= THIS_MODULE,
	.fb_setcolreg	= s3c_lcdfb_setcolreg,
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,
	.fb_open        = mylcd_open,
	.fb_release     = mylcd_release,
};


static struct fb_info *s3c_lcd;
static volatile unsigned long *gpbcon;
static volatile unsigned long *gpbdat;
static volatile unsigned long *gpccon;
static volatile unsigned long *gpdcon;
static volatile unsigned long *gpgcon;
static volatile struct lcd_regs* lcd_regs;
static u32 pseudo_palette[16];

static struct lcd_regs lcd_regs_backup;


/* from pxafb.c */
static inline unsigned int chan_to_field(unsigned int chan, struct fb_bitfield *bf)
{
	chan &= 0xffff;
	chan >>= 16 - bf->length;
	return chan << bf->offset;
}


static int s3c_lcdfb_setcolreg(unsigned int regno, unsigned int red,
			     unsigned int green, unsigned int blue,
			     unsigned int transp, struct fb_info *info)
{
	unsigned int val;
	
	if (regno > 16)
		return 1;

	/* ��red,green,blue��ԭɫ�����val */
	val  = chan_to_field(red,	&info->var.red);
	val |= chan_to_field(green, &info->var.green);
	val |= chan_to_field(blue,	&info->var.blue);
	
	//((u32 *)(info->pseudo_palette))[regno] = val;
	pseudo_palette[regno] = val;
	return 0;
}


static int lcd_suspend_notifier(struct notifier_block *nb,
				unsigned long event,
				void *dummy)
{

	switch (event) {
	case PM_SUSPEND_PREPARE:
		printk("lcd suspend notifiler test: PM_SUSPEND_PREPARE\n");
		return NOTIFY_OK;
	case PM_POST_SUSPEND:
		printk("lcd suspend notifiler test: PM_POST_SUSPEND\n");
		return NOTIFY_OK;

	default:
		return NOTIFY_DONE;
	}
}


static struct notifier_block lcd_pm_notif_block = {
	.notifier_call = lcd_suspend_notifier,
};

static void lcd_release(struct device * dev)
{
}

static struct platform_device lcd_dev = {
    .name         = "mylcd",
    .id       = -1,
    .dev = { 
    	.release = lcd_release, 
	},
};
static int lcd_probe(struct platform_device *pdev)
{
	pm_runtime_set_active(&pdev->dev);
	pm_runtime_use_autosuspend(&pdev->dev);
	pm_runtime_enable(&pdev->dev);
	return 0;
}
static int lcd_remove(struct platform_device *pdev)
{
	pm_runtime_disable(&pdev->dev);
	return 0;
}
static int lcd_suspend(struct device *dev)
{
	int i;
	unsigned long *dest = &lcd_regs_backup;
	unsigned long *src  = lcd_regs;
	
	for (i = 0; i < sizeof(lcd_regs_backup)/sizeof(unsigned long); i++)
	{
		dest[i] = src[i];
	}
	
	lcd_regs->lcdcon1 &= ~(1<<0); /* �ر�LCD���� */
	*gpbdat &= ~1;     /* �رձ��� */
	return 0;
}

static int lcd_resume(struct device *dev)
{
	int i;
	unsigned long *dest = lcd_regs;
	unsigned long *src  = &lcd_regs_backup;

	struct clk *clk = clk_get(NULL, "lcd");
	clk_enable(clk);
	clk_put(clk);
#if 0
	for (i = 0; i < sizeof(lcd_regs_backup)/sizeof(unsigned long); i++)
	{
		dest[i] = src[i];
	}
#else
	lcd_regs->lcdcon1 = lcd_regs_backup.lcdcon1 & ~1;
	lcd_regs->lcdcon2 = lcd_regs_backup.lcdcon2;
	lcd_regs->lcdcon3 = lcd_regs_backup.lcdcon3;
	lcd_regs->lcdcon4 = lcd_regs_backup.lcdcon4;
	lcd_regs->lcdcon5 = lcd_regs_backup.lcdcon5;

	lcd_regs->lcdsaddr1 = lcd_regs_backup.lcdsaddr1;
	lcd_regs->lcdsaddr2 = lcd_regs_backup.lcdsaddr2;
	lcd_regs->lcdsaddr3 = lcd_regs_backup.lcdsaddr3;
#endif
	lcd_regs->lcdcon1 |= (1<<0); /* ʹ��LCD������ */
	lcd_regs->lcdcon5 |= (1<<3); /* ʹ��LCD���� */
	*gpbdat |= 1;     /* ����ߵ�ƽ, ʹ�ܱ��� */		
	return 0;
}

static struct dev_pm_ops lcd_pm = {
	.suspend = lcd_suspend,
	.resume  = lcd_resume,	
	.runtime_suspend = lcd_suspend,
	.runtime_resume  = lcd_resume,	
};

struct platform_driver lcd_drv = {
	.probe		= lcd_probe,
	.remove		= lcd_remove,
	.driver		= {
		.name	= "mylcd",
		.pm     = &lcd_pm,
	}
};


static int lcd_init(void)
{	
	/* 1. ����һ��fb_info */
	s3c_lcd = framebuffer_alloc(0, NULL);

	/* 2. ���� */
	/* 2.1 ���ù̶��Ĳ��� */
	strcpy(s3c_lcd->fix.id, "mylcd");
	s3c_lcd->fix.smem_len = 480*272*16/8;
	s3c_lcd->fix.type     = FB_TYPE_PACKED_PIXELS;
	s3c_lcd->fix.visual   = FB_VISUAL_TRUECOLOR; /* TFT */
	s3c_lcd->fix.line_length = 480*2;
	
	/* 2.2 ���ÿɱ�Ĳ��� */
	s3c_lcd->var.xres           = 480;
	s3c_lcd->var.yres           = 272;
	s3c_lcd->var.xres_virtual   = 480;
	s3c_lcd->var.yres_virtual   = 272;
	s3c_lcd->var.bits_per_pixel = 16;

	/* RGB:565 */
	s3c_lcd->var.red.offset     = 11;
	s3c_lcd->var.red.length     = 5;
	
	s3c_lcd->var.green.offset   = 5;
	s3c_lcd->var.green.length   = 6;

	s3c_lcd->var.blue.offset    = 0;
	s3c_lcd->var.blue.length    = 5;

	s3c_lcd->var.activate       = FB_ACTIVATE_NOW;
	
	
	/* 2.3 ���ò������� */
	s3c_lcd->fbops              = &s3c_lcdfb_ops;
	
	/* 2.4 ���������� */
	s3c_lcd->pseudo_palette = pseudo_palette;
	//s3c_lcd->screen_base  = ;  /* �Դ�������ַ */ 
	s3c_lcd->screen_size   = 480*272*16/8;

	/* 3. Ӳ����صĲ��� */
	/* 3.1 ����GPIO����LCD */
	gpbcon = ioremap(0x56000010, 8);
	gpbdat = gpbcon+1;
	gpccon = ioremap(0x56000020, 4);
	gpdcon = ioremap(0x56000030, 4);
	gpgcon = ioremap(0x56000060, 4);

    *gpccon  = 0xaaaaaaaa;   /* GPIO�ܽ�����VD[7:0],LCDVF[2:0],VM,VFRAME,VLINE,VCLK,LEND */
	*gpdcon  = 0xaaaaaaaa;   /* GPIO�ܽ�����VD[23:8] */
	
	*gpbcon &= ~(3);  /* GPB0����Ϊ������� */
	*gpbcon |= 1;
	*gpbdat &= ~1;     /* ����͵�ƽ */

	*gpgcon |= (3<<8); /* GPG4����LCD_PWREN */
	
	/* 3.2 ����LCD�ֲ�����LCD������, ����VCLK��Ƶ�ʵ� */
	lcd_regs = ioremap(0x4D000000, sizeof(struct lcd_regs));

	/* bit[17:8]: VCLK = HCLK / [(CLKVAL+1) x 2], LCD�ֲ�P14
	 *            10MHz(100ns) = 100MHz / [(CLKVAL+1) x 2]
	 *            CLKVAL = 4
	 * bit[6:5]: 0b11, TFT LCD
	 * bit[4:1]: 0b1100, 16 bpp for TFT
	 * bit[0]  : 0 = Disable the video output and the LCD control signal.
	 */
	lcd_regs->lcdcon1  = (4<<8) | (3<<5) | (0x0c<<1);

#if 1
	/* ��ֱ�����ʱ�����
	 * bit[31:24]: VBPD, VSYNC֮���ٹ��೤ʱ����ܷ�����1������
	 *             LCD�ֲ� T0-T2-T1=4
	 *             VBPD=3
	 * bit[23:14]: ������, 320, ����LINEVAL=320-1=319
	 * bit[13:6] : VFPD, �������һ������֮���ٹ��೤ʱ��ŷ���VSYNC
	 *             LCD�ֲ�T2-T5=322-320=2, ����VFPD=2-1=1
	 * bit[5:0]  : VSPW, VSYNC�źŵ�������, LCD�ֲ�T1=1, ����VSPW=1-1=0
	 */
	lcd_regs->lcdcon2  = (1<<24) | (271<<14) | (1<<6) | (9);


	/* ˮƽ�����ʱ�����
	 * bit[25:19]: HBPD, VSYNC֮���ٹ��೤ʱ����ܷ�����1������
	 *             LCD�ֲ� T6-T7-T8=17
	 *             HBPD=16
	 * bit[18:8]: ������, 240, ����HOZVAL=240-1=239
	 * bit[7:0] : HFPD, �������һ�������һ����������֮���ٹ��೤ʱ��ŷ���HSYNC
	 *             LCD�ֲ�T8-T11=251-240=11, ����HFPD=11-1=10
	 */
	lcd_regs->lcdcon3 = (1<<19) | (479<<8) | (1);

	/* ˮƽ�����ͬ���ź�
	 * bit[7:0]	: HSPW, HSYNC�źŵ�������, LCD�ֲ�T7=5, ����HSPW=5-1=4
	 */	
	lcd_regs->lcdcon4 = 40;

#else
lcd_regs->lcdcon2 =	S3C2410_LCDCON2_VBPD(5) | \
		S3C2410_LCDCON2_LINEVAL(319) | \
		S3C2410_LCDCON2_VFPD(3) | \
		S3C2410_LCDCON2_VSPW(1);

lcd_regs->lcdcon3 =	S3C2410_LCDCON3_HBPD(10) | \
		S3C2410_LCDCON3_HOZVAL(239) | \
		S3C2410_LCDCON3_HFPD(1);

lcd_regs->lcdcon4 =	S3C2410_LCDCON4_MVAL(13) | \
		S3C2410_LCDCON4_HSPW(0);

#endif
	/* �źŵļ��� 
	 * bit[11]: 1=565 format
	 * bit[10]: 0 = The video data is fetched at VCLK falling edge
	 * bit[9] : 1 = HSYNC�ź�Ҫ��ת,���͵�ƽ��Ч 
	 * bit[8] : 1 = VSYNC�ź�Ҫ��ת,���͵�ƽ��Ч 
	 * bit[6] : 0 = VDEN���÷�ת
	 * bit[3] : 0 = PWREN���0
	 * bit[1] : 0 = BSWP
	 * bit[0] : 1 = HWSWP 2440�ֲ�P413
	 */
	lcd_regs->lcdcon5 = (1<<11) | (0<<10) | (1<<9) | (1<<8) | (1<<0);
	
	/* 3.3 �����Դ�(framebuffer), ���ѵ�ַ����LCD������ */
	s3c_lcd->screen_base = dma_alloc_writecombine(NULL, s3c_lcd->fix.smem_len, &s3c_lcd->fix.smem_start, GFP_KERNEL);
	
	lcd_regs->lcdsaddr1  = (s3c_lcd->fix.smem_start >> 1) & ~(3<<30);
	lcd_regs->lcdsaddr2  = ((s3c_lcd->fix.smem_start + s3c_lcd->fix.smem_len) >> 1) & 0x1fffff;
	lcd_regs->lcdsaddr3  = (480*16/16);  /* һ�еĳ���(��λ: 2�ֽ�) */	
	
	//s3c_lcd->fix.smem_start = xxx;  /* �Դ�������ַ */
	/* ����LCD */
	lcd_regs->lcdcon1 |= (1<<0); /* ʹ��LCD������ */
	lcd_regs->lcdcon5 |= (1<<3); /* ʹ��LCD���� */
	*gpbdat |= 1;     /* ����ߵ�ƽ, ʹ�ܱ��� */		

	/* 4. ע�� */
	register_framebuffer(s3c_lcd);

	/* ��Դ���� */
	register_pm_notifier(&lcd_pm_notif_block);

	platform_device_register(&lcd_dev);
	platform_driver_register(&lcd_drv);
	
	return 0;
}

static void lcd_exit(void)
{
	unregister_framebuffer(s3c_lcd);
	lcd_regs->lcdcon1 &= ~(1<<0); /* �ر�LCD���� */
	*gpbdat &= ~1;     /* �رձ��� */
	dma_free_writecombine(NULL, s3c_lcd->fix.smem_len, s3c_lcd->screen_base, s3c_lcd->fix.smem_start);
	iounmap(lcd_regs);
	iounmap(gpbcon);
	iounmap(gpccon);
	iounmap(gpdcon);
	iounmap(gpgcon);
	framebuffer_release(s3c_lcd);
	
	unregister_pm_notifier(&lcd_pm_notif_block);
	platform_device_unregister(&lcd_dev);
	platform_driver_unregister(&lcd_drv);
}

module_init(lcd_init);
module_exit(lcd_exit);

MODULE_LICENSE("GPL");


