#include <linux/clk.h>
#include <linux/gpio.h>
#include <linux/module.h>

#include <sound/soc.h>


/* �ο�sound\soc\samsung\s3c24xx_uda134x.c
 */

/*
 * 1. ����ע��һ����Ϊsoc-audio��ƽ̨�豸
 * 2. ���ƽ̨�豸��һ��˽������ snd_soc_card
 *    snd_soc_card����һ��snd_soc_dai_link
 *    snd_soc_dai_link����������ASOC�����ֵ�����
 */

static struct snd_soc_ops s3c2440_uda1341_ops = {
	//.hw_params = s3c24xx_uda134x_hw_params,
};

static struct snd_soc_dai_link s3c2440_uda1341_dai_link = {
	.name = "100ask_UDA1341",
	.stream_name = "100ask_UDA1341",
	.codec_name = "wm8976-codec",
	.codec_dai_name = "wm8976-iis",
	.cpu_dai_name = "s3c2440-iis",
	.ops = &s3c2440_uda1341_ops,
	.platform_name	= "s3c2440-dma",
};


static struct snd_soc_card myalsa_card = {
	.name = "S3C2440_UDA1341",
	.owner = THIS_MODULE,
	.dai_link = &s3c2440_uda1341_dai_link,
	.num_links = 1,
};

static void asoc_release(struct device * dev)
{
}

static struct platform_device asoc_dev = {
    .name         = "soc-audio",
    .id       = -1,
    .dev = { 
    	.release = asoc_release, 
	},
};

static int s3c2440_uda1341_init(void)
{
	platform_set_drvdata(&asoc_dev, &myalsa_card);
    platform_device_register(&asoc_dev);    
    return 0;
}

static void s3c2440_uda1341_exit(void)
{
    platform_device_unregister(&asoc_dev);
}

module_init(s3c2440_uda1341_init);
module_exit(s3c2440_uda1341_exit);

MODULE_LICENSE("GPL");
