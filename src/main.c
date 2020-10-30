#include <display.h>
#include <gdt.h>
#include <idt.h>
#include <irq.h>
#include <isrs.h>
#include <kb.h>
#include <libs.h>
#include <memory.h>
#include <mouse.h>
#include <multiboot.h>
#include <rtc.h>
#include <serial.h>
#include <stdio.h>
#include <system.h>
#include <timer.h>

int kmain(unsigned long magic, unsigned long addr) {
	multiboot_info_t *mbi = (multiboot_info_t *) addr;

	install_display(mbi->framebuffer_addr, mbi->framebuffer_width, mbi->framebuffer_height, mbi->framebuffer_bpp, mbi->framebuffer_pitch, mbi->framebuffer_type == 1 ? false : true);

	puts("Setting up basic serial interface (COM 1)...\n");
	init_serial();
	
	printf("Booted by \"%s\" FB type: %i cmdline: \"%s\" magic: 0x%x\n", mbi->boot_loader_name, mbi->framebuffer_type, mbi-> cmdline, magic);

	printf("t/OS test build, compiled on %s\n", __DATE__ );

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		printf("Warning: Boot magic value is 0x%x instead of the expected value: 0x%x\n", magic, MULTIBOOT_BOOTLOADER_MAGIC);
	}
	puts("Installing GDT...\n");
	gdt_install();
	puts("Installing IDT...\n");
	idt_install();
	puts("Installing ISRs...\n");
	isrs_install();
	puts("Installing IRQs...\n");
	irq_install();
	puts("Setting up RTC...\n");
	rtc_install();
	timer_phase(1024);
	timer_install();
	puts("Setting up Keyboard Controller...\n");
	keyboard_install();
	puts("Setting up Mouse Controller...\n");
	mouse_install();
	puts("Setting Interrupt Flag...\n");
	__asm__ __volatile__ ("sti"); 
	printf("Testing printf: char: %c, string: %s, int: %i, negative int: %i, hex: 0x%x, hex 2: 0x%x, float: %f\n", '!', "Hello world", 42, -10, 0xabcdef12, 0xcafe, 0.123);
	puts("Initializing Memory Manager\n");
	install_memory(mbi->mmap_addr, mbi->mmap_length, (uint32*) kmain);

	puts("Done.\n");
	
	for(;;);
	return 0;
}