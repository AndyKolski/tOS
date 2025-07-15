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

#define I8042_BUFFER         0x60
#define I8042_STATUS_COMMAND 0x64 // Reads are status, writes go to command

#define I8042_RESET 0xFE
void reboot() {
	cli();

	// Wait for input buffer to clear
	while (inb(I8042_STATUS_COMMAND) & 0x02) {}

	// Send "Write output port" command
	outb(I8042_STATUS_COMMAND, 0xD1);

	// Wait again for input buffer to clear
	while (inb(I8042_STATUS_COMMAND) & 0x02) {}

	// Send data with reset bit (bit 0) cleared to cause CPU reset
	outb(I8042_BUFFER, 0x00);

	// Use the Reset Control Register as a fallback
	outb(0xCF9, 0x06);

	// If that didn't work, try triple faulting the cpu
	uint8_t lidt_buffer[10] = {0};
	asm volatile("lidt %0" : : "m"(lidt_buffer));
	asm volatile("int3");

	// Just in case, we halt instead of returning.
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
