#include <header.h>
#include <stdint.h>

uint8 *memcpy(uint8 *dest, const uint8 *src, uint32 count)
{
	for (int i = 0; i < count; ++i) {
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

unsigned char inportb (unsigned short _port)
{
	unsigned char rv;
	__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}
static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}
void outportb (unsigned short _port, unsigned char _data)
{
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}
static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
    /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
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
	unsigned v;
	int sign = (radix == 10 && value < 0);    
	if (sign)
		v = -value;
	else
		v = (unsigned)value;

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

#define PORT 0x3f8   /* COM1 */
void init_serial() {
   outb(PORT + 1, 0x00);    // Disable all interrupts
   outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outb(PORT + 1, 0x00);    //                  (hi byte)
   outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

int is_transmit_empty() {
   return inb(PORT + 5) & 0x20;
}
 
void write_serial(char a) {
   while (is_transmit_empty() == 0);
 
   outb(PORT,a);
}

int serial_received() {
   return inb(PORT + 5) & 1;
}
 
char read_serial() {
   while (serial_received() == 0);
 
   return inb(PORT);
}
void serial_puts(char *text) {
	for (int i = 0; i < strlen(text); ++i) {
		write_serial(text[i]);
	}
}