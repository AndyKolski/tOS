#include <io.h>
#include <stddef.h>
#include <string.h>
#include <system.h>

void reverse(char s[]) {
	 int i, j;
	 char c;
 
	 for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
		 c = s[i];
		 s[i] = s[j];
		 s[j] = c;
	 }
 }

int itoa(int64 value, char *sp, int radix) {
	char tmp[66];
	char *tp = tmp;
	int i;
	uint64 v;
	int sign = (radix == 10 && value < 0);    
	if (sign)
		v = -value;
	else
		v = (uint64)value;

	while (v || tp == tmp)
	{
		i = v % radix;
		v /= radix; // v/=radix uses less CPU clocks than v=v/radix does
		if (i < 10) 
		  *tp++ = i+'0';
		else
		  *tp++ = i + 'A' - 10;
	}

	int len = tp - tmp;

	if (sign) 
	{
		*sp++ = '-';
		len++;
	}

	while (tp > tmp)
		*sp++ = *--tp;

	return len;
}

int isdigit (char c) {
    if ((c>='0') && (c<='9')) return 1;
    return 0;
}

void halt() {
	asm volatile ("cli");
	while (true) {
		asm volatile ("hlt");
	}
}

void reboot() {
	 uint8_t good = 0x02;
	 while (good & 0x02)
		  good = inb(0x64);
	 outb(0x64, 0xFE);
	 halt();
}