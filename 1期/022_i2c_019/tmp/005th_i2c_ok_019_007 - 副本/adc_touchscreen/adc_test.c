
#include "adc.h"

void adc_test(void)
{
	int val;
	double vol;
	int m; /* �������� */
	int n; /* С������ */
	
	adc_init();

	while (1)
	{
		val = adc_read_ain0();
		vol = (double)val/1023*3.3;   /* 1023----3.3v */
		m = (int)vol;   /* 3.01, m = 3 */
		vol = vol - m;  /* С������: 0.01 */
		n = vol * 1000;  /* 10 */

		/* �ڴ����ϴ�ӡ */
		printf("vol: %d.%03dv\r", m, n);  /* 3.010v */

		/* ��LCD�ϴ�ӡ */
		//fb_print_string();
	}
}

