#include <system.h>

uint8 inb(uint16 port) {
	 uint8 ret;
	 __asm__ volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
	 return ret;
}
void outb(uint16 port, uint8 val) {
	 __asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

uint16 inw(uint16 port) {
	 uint16 ret;
	 __asm__ volatile ( "inw %1, %0" : "=a"(ret) : "Nd"(port) );
	 return ret;
}
void outw(uint16 port, uint16 val) {
	 __asm__ volatile ( "outw %0, %1" : : "a"(val), "Nd"(port) );
}

uint32 inl(uint16 port) {
	 uint32 ret;
	 __asm__ volatile ( "inl %1, %0" : "=a"(ret) : "Nd"(port) );
	 return ret;
}
void outl(uint16 port, uint32 val) {
	 __asm__ volatile ( "outl %0, %1" : : "a"(val), "Nd"(port) );
}

void io_wait() {
    outb(0x80, 0);
}