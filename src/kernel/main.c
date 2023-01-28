#include <display.h>
#include <interrupts/idt.h>
#include <interrupts/irq.h>
#include <interrupts/isrs.h>
#include <keyboard.h>
#include <memory/paging.h>
#include <memory/pmm.h>
#include <mouse.h>
#include <multibootdata.h>
#include <serial.h>
#include <stdio.h>
#include <system.h>
#include <time/time.h>

#include <pcspeaker.h>


int kmain(uint32 bootloaderMagic, uint32 multibootLocation) {
	
	puts("kmain() function called.");


	puts("Initializing IDT...");
	initIDT();
	puts("Initializing ISRs...");
	initISRs();
	puts("Initializing IRQs...");
	initIRQs();

	puts("Initializing serial interface...");
	initSerial();

	puts("Initializing basic paging...");
	initPaging();
	

	puts("Parsing multiboot data...");
	parseMultibootData(bootloaderMagic, multibootLocation);
	bootData_t *bootData = getBootData();

	puts("Initializing display...");
	initDisplay();

	printf("t/OS " __ARCH " test build " __GIT_VERSION ", compiled on " __DATE__ " at " __TIME__ " with " __CC_VERSION"\n");
	printf("Booted by \"%s\" cmdline: \"%s\" magic: 0x%x\n", bootData->bootloaderName, bootData->cmdline, bootloaderMagic);

	printf("Offset: 0x%lx\n", KERNEL_OFFSET);
	
	puts("Initializing memory management...");
	initPMM();
	
	puts("Initializing time...");
	initTime();
	puts("Initializing mouse controller...");
	initMouse();
	puts("Initializing keyboard controller...");
	initKeyboard();
	puts("Enabling interrupts...");
	sti();

	printf("Testing printf: char: %c, string: %s, int: %i, negative int: %i, hex: 0x%x, hex 2: 0x%x\n", '!', "Hello world", 42, -10, 0xabcdef12, 0xcafe);

	puts("OK");

	
	while (true) {
		__asm__ volatile ("hlt"); // nothing to do, so we halt the processor until something needs to happen, which we deal with, and then halt again
	}
	return 0;
}