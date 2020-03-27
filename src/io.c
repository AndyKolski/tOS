#include <system.h>

uint8 inportb (uint16 _port) {
	unsigned char rv;
	__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}
void outportb (uint16 _port, uint8 _data) {
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}
uint8_t inb(uint16 port) {
	 uint8_t ret;
	 asm volatile ( "inb %1, %0"
						 : "=a"(ret)
						 : "Nd"(port) );
	 return ret;
}
void outb(uint16 port, uint8 val) {
	 asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
	 /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
	  * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
	  * The  outb  %al, %dx  encoding is the only option for all other cases.
	  * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}