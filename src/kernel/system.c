#include <io.h>
#include <stdio.h>
#include <system.h>


void sti() {
	__asm__ __volatile__ ("sti"); 
}
void cli() {
	__asm__ __volatile__ ("cli"); 
}

void halt() {
	cli();
	while (true) {
		__asm__ volatile ("hlt");
	}
}

void reboot() {
	 uint8_t good = 0x02;
	 while (good & 0x02)
		  good = inb(0x64);
	 outb(0x64, 0xFE);
	 halt();
}

void _assert(char *file, uint32 line, const char *func, char *msg, bool conf) {
	if (!conf) {
		printf("\n [!!!] Assertion failed at %s:%u in function %s - %s\n", file, line, func, msg);
		halt();
	}
}

void _panic(char *file, uint32 line, const char *func, char *msg) {
	printf("\n [!!!] Kernel panic at %s:%u in function %s - %s\n", file, line, func, msg);
	halt();
}

void panicNoLineNumber(char *msg) {
	printf("\n [!!!] Kernel panic - %s\n", msg);
	halt();
}

uintptr_t __stack_chk_guard = 0x681a7261; // Random number, picked using random.org
void __stack_chk_fail(void) {
	panicNoLineNumber("Stack smashing detected!");	
}

