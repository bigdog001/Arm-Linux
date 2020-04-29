
void do_write_at24cxx(void)
{
	unsigned int addr;
	unsigned char str[100];
	int err;
	
	/* ��õ�ַ */
	printf("Enter the address of sector to write: ");
	addr = get_uint();

	if (addr > 256)
	{
		printf("address > 256, error!\n\r");
		return;
	}

	printf("Enter the string to write: ");
	gets(str);

	printf("writing ...\n\r");
	err = at24cxx_write(addr, str, strlen(str)+1);
	printf("at24cxx_write ret = %d\n\r", err);
}

void do_read_at24cxx(void)
{
	unsigned int addr;
	int i, j;
	unsigned char c;
	unsigned char data[100];
	unsigned char str[16];
	int len;
	int err;
	int cnt = 0;
	
	/* ��õ�ַ */
	printf("Enter the address to read: ");
	addr = get_uint();

	if (addr > 256)
	{
		printf("address > 256, error!\n\r");
		return;
	}

	/* ��ó��� */
	printf("Enter the length to read: ");
	len = get_int();

	err = at24cxx_read(addr, data, len);
	printf("at24cxx_read ret = %d\n\r", err);

	printf("Data : \n\r");
	/* ���ȹ̶�Ϊ64 */
	for (i = 0; i < 4; i++)
	{
		/* ÿ�д�ӡ16������ */
		for (j = 0; j < 16; j++)
		{
			/* �ȴ�ӡ��ֵ */
			c = data[cnt++];
			str[j] = c;
			printf("%02x ", c);
		}

		printf("   ; ");

		for (j = 0; j < 16; j++)
		{
			/* ���ӡ�ַ� */
			if (str[j] < 0x20 || str[j] > 0x7e)  /* �������ַ� */
				putchar('.');
			else
				putchar(str[j]);
		}
		printf("\n\r");
	}
}


void i2c_test(void)
{
	char c;

	/* ��ʼ�� */
	i2c_init();

	while (1)
	{
		/* ��ӡ�˵�, ������ѡ��������� */
		printf("[w] Write at24cxx\n\r");
		printf("[r] Read at24cxx\n\r");
		printf("[q] quit\n\r");
		printf("Enter selection: ");

		c = getchar();
		printf("%c\n\r", c);

		/* ��������:
		 * 3. ��дĳ����ַ
		 * 4. ��ĳ����ַ
		 */
		switch (c)		 
		{
			case 'q':
			case 'Q':
				return;
				break;
				
			case 'w':
			case 'W':
				do_write_at24cxx();
				break;

			case 'r':
			case 'R':
				do_read_at24cxx();
				break;
			default:
				break;
		}
	}
}


