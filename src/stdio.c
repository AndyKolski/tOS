#include <ctype.h>
#include <display.h>
#include <io.h>
#include <serial.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

kchar putchar(kchar chr) {
	
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

void puts(kchar *text) {
	for (uint32 i = 0; i < strlen(text); ++i) {
		putchar(text[i]);
	}
	putchar('\n');
}

// Note:
// This printf implementation is based on one that was posted to the osdev.org wiki by user A22347
// on September 19th, 2017. It is in the public domain, as anything posted to the wiki after 
// June 6th, 2011 must use the Creative Commons CC0 designation, which places work into the public 
// domain.

// https://wiki.osdev.org/index.php?title=User:A22347/Printf

// http://creativecommons.org/publicdomain/zero/1.0/

// https://wiki.osdev.org/OSDev_Wiki:Copyrights
// https://wiki.osdev.org/OSDev_Wiki:License

char* __int_str(intmax_t i, char b[], int base, bool plusSignIfNeeded, bool spaceSignIfNeeded,
				int paddingNo, bool justify, bool zeroPad) {
 
	char digit[32] = {0};
	memset(digit, 0, 32);
	strcpy(digit, "0123456789");
 
	if (base == 16) {
		strcat(digit, "ABCDEF");
	} else if (base == 17) {
		strcat(digit, "abcdef");
		base = 16;
	}
 
	char* p = b;
	if (i < 0) {
		*p++ = '-';
		i *= -1;
	} else if (plusSignIfNeeded) {
		*p++ = '+';
	} else if (!plusSignIfNeeded && spaceSignIfNeeded) {
		*p++ = ' ';
	}
 
	intmax_t shifter = i;
	do {
		++p;
		shifter = shifter / base;
	} while (shifter);
 
	*p = '\0';
	do {
		*--p = digit[i % base];
		i = i / base;
	} while (i);
 
	int padding = paddingNo - (int)strlen(b);
	if (padding < 0) padding = 0;
 
	if (justify) {
		while (padding--) {
			if (zeroPad) {
				b[strlen(b)] = '0';
			} else {
				b[strlen(b)] = ' ';
			}
		}
 
	} else {
		char a[256] = {0};
		while (padding--) {
			if (zeroPad) {
				a[strlen(a)] = '0';
			} else {
				a[strlen(a)] = ' ';
			}
		}
		strcat(a, b);
		strcpy(b, a);
	}
 
	return b;
}
 
void displayCharacter(char c, int32* a) {
	putchar(c);
	*a += 1;
}
 
void displayString(char* c, int32* a) {
	for (int i = 0; c[i]; ++i) {
		displayCharacter(c[i], a);
	}
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough=" // this section causes GCC warnings. We disable them 
int vprintf (const char* format, va_list list) {
	int32 chars        = 0;
	char intStrBuffer[256] = {0};
 
	for (int i = 0; format[i]; ++i) {
 
		char specifier   = '\0';
		char length      = '\0';
 
		int  lengthSpec  = 0; 
		int  precSpec    = 0;
		bool leftJustify = false;
		bool zeroPad     = false;
		bool spaceNoSign = false;
		bool altForm     = false;
		bool plusSign    = false;
		bool emode       = false;
		int  expo        = 0;
 
		if (format[i] == '%') {
			++i;
 
			bool extBreak = false;
			while (1) {
 
				switch (format[i]) {
					case '-':
						leftJustify = true;
						++i;
						break;
 
					case '+':
						plusSign = true;
						++i;
						break;
 
					case '#':
						altForm = true;
						++i;
						break;
 
					case ' ':
						spaceNoSign = true;
						++i;
						break;
 
					case '0':
						zeroPad = true;
						++i;
						break;
 
					default:
						extBreak = true;
						break;
				}
 
				if (extBreak) break;
			}
 
			while (isDigit(format[i])) {
				lengthSpec *= 10;
				lengthSpec += format[i] - 48;
				++i;
			}
 
			if (format[i] == '*') {
				lengthSpec = va_arg(list, int);
				++i;
			}
 
			if (format[i] == '.') {
				++i;
				while (isDigit(format[i])) {
					precSpec *= 10;
					precSpec += format[i] - 48;
					++i;
				}
 
				if (format[i] == '*') {
					precSpec = va_arg(list, int);
					++i;
				}
			} else {
				precSpec = 6;
			}
 
			if (format[i] == 'h' || format[i] == 'l' || format[i] == 'j' ||
				   format[i] == 'z' || format[i] == 't' || format[i] == 'L' || format[i] == 'q') {
				length = format[i];
				++i;
				if (format[i] == 'h') {
					length = 'H';
				} else if (format[i] == 'l') {
					length = 'q';
				}
			}
			specifier = format[i];
 
			memset(intStrBuffer, 0, 256);
 
			int base = 10;
			if (specifier == 'o') {
				base = 8;
				specifier = 'u';
				if (altForm) {
					displayString("0", &chars);
				}
			}
			if (specifier == 'p') {
				base = 16;
				length = 'z';
				specifier = 'u';
			}
			switch (specifier) {
				case 'X':
					base = 16;
				case 'x':
					base = base == 10 ? 17 : base;
					if (altForm) {
						displayString("0x", &chars);
					}
 
				case 'u':
				{
					switch (length) {
						case 0:
						{
							unsigned int integer = va_arg(list, unsigned int);
							__int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
							displayString(intStrBuffer, &chars);
							break;
						}
						case 'H':
						{
							unsigned char integer = (unsigned char) va_arg(list, unsigned int);
							__int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
							displayString(intStrBuffer, &chars);
							break;
						}
						case 'h':
						{
							unsigned short int integer = va_arg(list, unsigned int);
							__int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
							displayString(intStrBuffer, &chars);
							break;
						}
						case 'l':
						{
							unsigned long integer = va_arg(list, unsigned long);
							__int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
							displayString(intStrBuffer, &chars);
							break;
						}
						case 'q':
						{
							unsigned long long integer = va_arg(list, unsigned long long);
							__int_str((int64)integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
							displayString(intStrBuffer, &chars);
							break;
						}
						case 'j':
						{
							uintmax_t integer = va_arg(list, uintmax_t);
							__int_str((int64)integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
							displayString(intStrBuffer, &chars);
							break;
						}
						case 'z':
						{
							size_t integer = va_arg(list, size_t);
							__int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
							displayString(intStrBuffer, &chars);
							break;
						}
						case 't':
						{
							ptrdiff_t integer = va_arg(list, ptrdiff_t);
							__int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
							displayString(intStrBuffer, &chars);
							break;
						}
						default:
							break;
					}
					break;
				}
 
				case 'd':
				case 'i':
				{
					switch (length) {
					case 0:
					{
						int integer = va_arg(list, int);
						__int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
						displayString(intStrBuffer, &chars);
						break;
					}
					case 'H':
					{
						signed char integer = (signed char) va_arg(list, int);
						__int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
						displayString(intStrBuffer, &chars);
						break;
					}
					case 'h':
					{
						short int integer = va_arg(list, int);
						__int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
						displayString(intStrBuffer, &chars);
						break;
					}
					case 'l':
					{
						long integer = va_arg(list, long);
						__int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
						displayString(intStrBuffer, &chars);
						break;
					}
					case 'q':
					{
						long long integer = va_arg(list, long long);
						__int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
						displayString(intStrBuffer, &chars);
						break;
					}
					case 'j':
					{
						intmax_t integer = va_arg(list, intmax_t);
						__int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
						displayString(intStrBuffer, &chars);
						break;
					}
					case 'z':
					{
						size_t integer = va_arg(list, size_t);
						__int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
						displayString(intStrBuffer, &chars);
						break;
					}
					case 't':
					{
						ptrdiff_t integer = va_arg(list, ptrdiff_t);
						__int_str(integer, intStrBuffer, base, plusSign, spaceNoSign, lengthSpec, leftJustify, zeroPad);
						displayString(intStrBuffer, &chars);
						break;
					}
					default:
						break;
					}
					break;
				}
 
				case 'c':
				{
					if (length == 'l') {
						displayCharacter(va_arg(list, long int), &chars);
					} else {
						displayCharacter(va_arg(list, int), &chars);
					}
 
					break;
				}
 
				case 's':
				{
					displayString(va_arg(list, char*), &chars);
					break;
				}
 
				case 'n':
				{
					switch (length) {
						case 'H':
							*(va_arg(list, signed char*)) = chars;
							break;
						case 'h':
							*(va_arg(list, short int*)) = chars;
							break;
 
						case 0: {
							int* a = va_arg(list, int*);
							*a = chars;
							break;
						}
 
						case 'l':
							*(va_arg(list, long*)) = chars;
							break;
						case 'q':
							*(va_arg(list, long long*)) = chars;
							break;
						case 'j':
							*(va_arg(list, intmax_t*)) = chars;
							break;
						case 'z':
							*(va_arg(list, size_t*)) = (uint32)chars;
							break;
						case 't':
							*(va_arg(list, ptrdiff_t*)) = chars;
							break;
						default:
							break;
					}
					break;
				}
 
				case 'e':
				case 'E':
					emode = true;
 
				case 'f':
				case 'F':
				case 'g':
				case 'G':
				{
					double floating = va_arg(list, double);
 
					while (emode && floating >= 10) {
						floating /= 10;
						++expo;
					}
 
					int form = lengthSpec - precSpec - expo - (precSpec || altForm ? 1 : 0);
					if (emode) {
						form -= 4;      // 'e+00'
					}
					if (form < 0) {
						form = 0;
					}
 
					__int_str(floating, intStrBuffer, base, plusSign, spaceNoSign, form, \
							  leftJustify, zeroPad);
 
					displayString(intStrBuffer, &chars);
 
					floating -= (int) floating;
 
					for (int o = 0; o < precSpec; ++o) {
						floating *= 10;
					}
					intmax_t decPlaces = (intmax_t) (floating + 0.5);
 
					if (precSpec) {
						displayCharacter('.', &chars);
						__int_str(decPlaces, intStrBuffer, 10, false, false, 0, false, false);
						intStrBuffer[precSpec] = 0;
						displayString(intStrBuffer, &chars);
					} else if (altForm) {
						displayCharacter('.', &chars);
					}
 
					break;
				}
 
 
				case 'a':
				case 'A':
					//ACK! Hexadecimal floating points...
					break;
 
				default:
					break;
			}
 
			if (specifier == 'e') {
				displayString("e+", &chars);
			} else if (specifier == 'E') {
				displayString("E+", &chars);
			}
 
			if (specifier == 'e' || specifier == 'E') {
				__int_str(expo, intStrBuffer, 10, false, false, 2, false, true);
				displayString(intStrBuffer, &chars);
			}
 
		} else {
			displayCharacter(format[i], &chars);
		}
	}
 
	return chars;
}
 
#pragma GCC diagnostic pop

int printf (const char* format, ...) {
	va_list list;
	va_start (list, format);
	int i = vprintf (format, list);
	va_end (list);
	return i;
 
}