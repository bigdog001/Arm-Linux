#include <s3c24xx.h>

extern void delay(void);

static void set_csb(int val)
{
    if (val)
    {
        GPBDAT |= (1<<2);
    }
    else
    {
        GPBDAT &= ~(1<<2);
    }
}

static void set_clk(int val)
{
    if (val)
    {
        GPBDAT |= (1<<4);
    }
    else
    {
        GPBDAT &= ~(1<<4);
    }
}

static void set_dat(int val)
{
    if (val)
    {
        GPBDAT |= (1<<3);
    }
    else
    {
        GPBDAT &= ~(1<<3);
    }
}


static void wm8976_write_reg(unsigned char reg, unsigned int data)
{
	int i;
	unsigned short val = (reg << 9) | (data & 0x1ff);

    set_csb(1);
    set_dat(1);
    set_clk(1);

	for (i = 0; i < 16; i++){
		if (val & (1<<15))
		{
            set_clk(0);
            set_dat(1);
			delay();
            set_clk(1);
		}
		else
		{
            set_clk(0);
            set_dat(0);
			delay();
            set_clk(1);
		}

		val = val << 1;
	}

    set_csb(0);
	delay();
    set_csb(1);
    set_dat(1);
    set_clk(1);
	
}


/* ͨ�����ƽӿ�(GPB2,3,4)��д�Ĵ������г�ʼ�� */
void wm8976_init(void)
{
	/* software reset */
	wm8976_write_reg(0, 0);

	/* OUT2����/��������
	 * ��/��ͨ�����������
	 * ��/��DAC��
	 */
	wm8976_write_reg(0x3, 0x6f);
	
	wm8976_write_reg(0x1, 0x1f);//biasen,BUFIOEN.VMIDSEL=11b  
	wm8976_write_reg(0x2, 0x185);//ROUT1EN LOUT1EN, inpu PGA enable ,ADC enable

	wm8976_write_reg(0x6, 0x0);//SYSCLK=MCLK  
	wm8976_write_reg(0x4, 0x10);//16bit 		
	wm8976_write_reg(0x2B,0x10);//BTL OUTPUT  
	wm8976_write_reg(0x9, 0x50);//Jack detect enable  
	wm8976_write_reg(0xD, 0x21);//Jack detect  
	wm8976_write_reg(0x7, 0x01);//Jack detect 
}

/*
 * volume : 0~100, 0��ʾ��С����
 */
void wm8976_set_volume(int volume)
{
    /* ioctl: valԽ���ʾ����Խ��, 0-��С, 100-���
     * WM8976: 52,53�żĴ���bit[5:0]��ʾ����, ֵԽ������Խ��, 0-63
     */
    int val = volume * 63 / 100;
    wm8976_write_reg(52, (1<<8)|val);
    wm8976_write_reg(53, (1<<8)|val);
}
    


