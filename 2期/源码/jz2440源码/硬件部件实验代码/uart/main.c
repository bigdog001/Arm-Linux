#include "serial.h"

int main()
{
    unsigned char c;
    uart0_init();   // ������115200��8N1(8������λ����У��λ��1��ֹͣλ)

    while(1)
    {
        // �Ӵ��ڽ������ݺ��ж����Ƿ����ֻ���ĸ���������1�����
        c = getc();
        if (isDigit(c) || isLetter(c))
            putc(c+1);
    }

    return 0;
}
