#include <io.h>
#include <stdio.h>
#include <system.h>

void halt() {
	__asm__ volatile ("cli");
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

void _assert(kchar *file, uint32 line, kchar *msg, bool conf) {
	if (!conf) {
		printf("\n [!!!] Assertion failed at %s:%lu - %s\n", file, line, msg);
		halt();
	}
}

void _panic(kchar *file, uint32 line, kchar *msg) {
	printf("\n [!!!] Kernel panic at %s:%lu - %s\n", file, line, msg);
	halt();
}

void panicNoLineNumber(kchar *msg) {
	printf("\n [!!!] Kernel panic - %s\n", msg);
	halt();
}

uintptr_t __stack_chk_guard = 0x681a7261; // Random number, picked using random.org
void __stack_chk_fail(void) {
	panicNoLineNumber("Stack smashing detected!");	
}