all: start.o led.o uart.o init.o main.o exception.o interrupt.o timer.o
	#arm-linux-ld -Ttext 0 -Tdata 0x30000000  start.o led.o uart.o init.o main.o -o sdram.elf
	arm-linux-ld -T sdram.lds $^ -o sdram.elf
	arm-linux-objcopy -O binary -S sdram.elf sdram.bin
	arm-linux-objdump -D sdram.elf > sdram.dis
clean:
	rm *.bin *.o *.elf *.dis
	
%.o : %.c
	arm-linux-gcc -c -o $@ $<

%.o : %.S
	arm-linux-gcc -c -o $@ $<
	