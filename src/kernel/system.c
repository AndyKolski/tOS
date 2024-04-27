#include <io.h>
#include <stdio.h>
#include <system.h>

uint64 intDivCeil(uint64 a, uint64 b) { // calculates ceil(a/b) without using any floating point math
	if (a % b == 0) {
		return a / b;
	} else {
		return a / b + 1;
	}
}

void sti() {
	asm volatile("sti");
}
void cli() {
	asm volatile("cli");
}

void halt() {
	cli();
	while (true) {
		asm volatile("hlt");
	}
}

void reboot() {
	uint8_t good = 0x02;
	while (good & 0x02)
		good = inb(0x64);
	outb(0x64, 0xFE);
	halt();
}

void _assert(const char *file, uint32 line, const char *func, const char *msg, bool conf) {
	if (!conf) {
		printf("\n [!!!] Assertion failed at %s:%u in function %s - %s\n", file, line, func, msg);
		halt();
	}
}

void _panic(const char *file, uint32 line, const char *func, const char *msg) {
	printf("\n [!!!] Kernel panic at %s:%u in function %s - %s\n", file, line, func, msg);
	halt();
}

void panicNoLineNumber(const char *msg) {
	printf("\n [!!!] Kernel panic - %s\n", msg);
	halt();
}

uintptr_t __stack_chk_guard = 0xc87cd33f0308ac33; // Random number, picked using random.org
void __stack_chk_fail(void) {
	panicNoLineNumber("Stack smashing detected!");
}
