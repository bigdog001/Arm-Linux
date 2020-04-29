
#include "s3c2440_soc.h"
#include "uart.h"
#include "init.h"

int main(void)
{
	unsigned char c;
	
	uart0_init();
	puts("Enter the Tacc val: \n\r");
	
	while(1)
	{
		c = getchar();
		putchar(c);
		if (c >= '0' && c <= '7')
		{
			bank0_tacc_set(c - '0');
			led_test();
		}
		else
		{
			puts("Error, val should between 0~7\n\r");
			puts("Enter the Tacc val: \n\r");
		}
	}
	return 0;
}



