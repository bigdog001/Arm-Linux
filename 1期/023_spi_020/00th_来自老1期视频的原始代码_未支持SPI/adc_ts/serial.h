void uart0_init(void);
void putc(unsigned char c);
unsigned char getc(void);
unsigned char awaitkey(unsigned long timeout);
int isDigit(unsigned char c);
int isLetter(unsigned char c);
