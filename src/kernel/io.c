#include <system.h>

uint8 inb(uint16 port) {
	 uint8 ret;
	 __asm__ volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
	 return ret;
}
void outb(uint16 port, uint8 val) {
	 __asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}
void io_wait() {
    outb(0x80, 0);
}