#include <display.h>
#include <io.h>
#include <irq.h>
#include <libs.h>
#include <string.h>
#include <system.h>

#define PORT 0x3f8   /* COM1 */
volatile bool IS_SERIAL_ENABLED = false;
 
int serial_received() {
	if (!IS_SERIAL_ENABLED) {return 0;}
	return inb(PORT + 5) & 1;
}
 
char read_serial() {
	if (!IS_SERIAL_ENABLED) {return 0;}
	while (serial_received() == 0) {} //wait until we get something
 
	return (char)inb(PORT);
}

int is_transmit_empty() {
	if (!IS_SERIAL_ENABLED) {return 1;}
	return inb(PORT + 5) & 0x20;
}

void serial_putc(char chr) {
	if (!IS_SERIAL_ENABLED) {return;}
	while (is_transmit_empty() == 0) {} //wait until the buffer is empty
 
	outb(PORT, (uint8)chr);
}

void serial_puts(char *text) {
	if (!IS_SERIAL_ENABLED) {return;}
	for (uint32 i = 0; i < strlen(text); ++i) {
		serial_putc(text[i]);
	}
}

void serial_handler(struct regs *r __attribute__((__unused__))) {
	char i = read_serial();
	if (i == '\b') {
		termBackspace();
	} else {
		// printf("c: %i ", i);
		putc(i);
	}
}

void init_serial() {
	IS_SERIAL_ENABLED = true;
	irq_install_handler(4, serial_handler);
	outb(PORT + 1, 0x01);    // Enable basic interrupts
	outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
	outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
	outb(PORT + 1, 0x00);    //                  (hi byte)
	outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
	outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set

	serial_puts("Serial connection initialized!\r\n");
}
