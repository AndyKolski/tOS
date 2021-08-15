#include <display.h>
#include <gdt.h>
#include <idt.h>
#include <irq.h>
#include <isrs.h>
#include <keyboard.h>
#include <libs.h>
#include <memory.h>
#include <mouse.h>
#include <multiboot.h>
#include <paging.h>
#include <serial.h>
#include <stdio.h>
#include <syscall.h>
#include <system.h>
#include <time.h>
#include <userland.h>

int kmain(unsigned long bootloaderMagic, multiboot_info_t* multibootInfo) {

	install_display(multibootInfo->framebuffer_addr, multibootInfo->framebuffer_width, multibootInfo->framebuffer_height, multibootInfo->framebuffer_bpp, multibootInfo->framebuffer_pitch, multibootInfo->framebuffer_type == 1 ? false : true);

	puts("Setting up serial interface...\n");
	init_serial();
	
	printf("Booted by \"%s\" FB type: %i cmdline: \"%s\" magic: 0x%lx\n", (char*)multibootInfo->boot_loader_name, multibootInfo->framebuffer_type, (char*)multibootInfo->cmdline, bootloaderMagic);

	printf("t/OS test build " GIT_VERSION ", compiled on " __DATE__ " at " __TIME__ " with " CC_VERSION  " \n");

	if (bootloaderMagic != MULTIBOOT_BOOTLOADER_MAGIC) {
		printf("Warning: Bootloader magic value is 0x%lx instead of the expected value: 0x%x\n", bootloaderMagic, MULTIBOOT_BOOTLOADER_MAGIC);
	}
	puts("Installing GDT...\n");
	gdt_install();
	puts("Installing IDT...\n");
	idt_install();
	puts("Installing ISRs...\n");
	isrs_install();
	puts("Installing syscall handler...\n");
	install_syscall();
	puts("Installing IRQs...\n");
	irq_install();
	puts("Initializing Time...\n");
	initTime();
	puts("Initializing Memory Management...\n");
	install_memory((multiboot_memory_map_t*) multibootInfo->mmap_addr, multibootInfo->mmap_length);
	puts("Enabling Paging... \n");
	install_paging();
	puts("Setting up Mouse Controller...\n");
	mouse_install();
	puts("Setting up Keyboard Controller...\n");
	keyboard_install();
	puts("Setting Interrupt Flag...\n");
	__asm__ __volatile__ ("sti"); 
	printf("Testing printf: char: %c, string: %s, int: %i, negative int: %i, hex: 0x%x, hex 2: 0x%x, float: %f\n", '!', "Hello world", 42, -10, 0xabcdef12, 0xcafe, 0.123);

	puts("OK. Jumping to userland (Ring 3)\n");

	start_userland();

	
	while (true) {
		__asm__ volatile ("hlt"); // nothing to do, so we halt the processor until something needs to happen, which we deal with, and then halt again
	}
	return 0;
}