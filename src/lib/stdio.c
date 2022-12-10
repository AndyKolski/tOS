#include <ctype.h>
#include <display.h>
#include <io.h>
#include <serial.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

char putchar(char chr) {
	
	outb(0xe9, (uint8)chr);
	
	if (chr == '\n') {
		serial_putc('\r');
		serial_putc('\n');
	} else {
		serial_putc(chr);
	}

	if (chr == '\b') {
		outb(0xe9, ' ');
		outb(0xe9, '\b');
		serial_putc(' ');
		serial_putc('\b');
		terminalBackspace();
	} else {
		terminalPrintChar(chr);
	}

	return chr;
}

void puts(char *text) {
	for (uint32 i = 0; i < strlen(text); ++i) {
		putchar(text[i]);
	}
	putchar('\n');
}
