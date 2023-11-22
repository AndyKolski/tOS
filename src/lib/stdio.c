#include <ctype.h>
#include <display.h>
#include <io.h>
#include <serial.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

/// @brief Prints a single character to the screen
/// @param chr The character to print
/// @return The character that was printed
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

/// @brief Prints a string to the screen, followed by a newline character
/// @param text The string to print
void puts(const char *text) {
	for (uint32 i = 0; i < strlen(text); ++i) {
		putchar(text[i]);
	}
	putchar('\n');
}
