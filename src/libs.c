#include <io.h>
#include <system.h>

uint8 *memcpy(uint8 *dest, const uint8 *src, uint32 count)
{
	for (uint32 i = 0; i < count; ++i) {
		dest[i] = src[i];
	}
	return dest;
}

void memcpyint(void *dest, void *src, int n) {
	int *csrc = (int *) src;
	int *cdest = (int *) dest;

	for (int i=0; i<n; i++)
		cdest[i] = csrc[i];
}

char *memset(char *dest, char val, int count)
{
	for (int i = 0; i < count; ++i) {
		dest[i] = val;
	}
	return dest;
}

short *memsetw(short *dest, short val, int count)
{
	for (int i = 0; i < count; ++i) {
		dest[i] = val;
	}
	return dest;
}

int strlen(const kchar *str)
{
	int l = 0;
	while (str[l] != 0) {
		l++;
	}
	return l;
}

char* strcpy(kchar* destination, const kchar* source) {
	// return if no memory is allocated to the destination
	if (destination == 0)
		return 0;

	// take a pointer pointing to the beginning of destination string
	char *ptr = destination;
	
	// copy the C-string pointed by source into the array
	// pointed by destination
	while (*source != 0)
	{
		*destination = *source;
		destination++;
		source++;
	}

	// include the terminating null character
	*destination = 0;

	// destination is returned by standard strcpy()
	return ptr;
}

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