#include <header.h>
#include <multiboot.h>

int kmain(unsigned long magic, unsigned long addr) {
	multiboot_info_t *mbi = (multiboot_info_t *) addr;
	install_display(mbi->framebuffer_addr, mbi->framebuffer_width, mbi->framebuffer_height, mbi->framebuffer_bpp, mbi->framebuffer_pitch);
	//drawDuck();
	puts("Installing GDT...\n");
	gdt_install();
	puts("Installing IDT...\n");
	idt_install();
	puts("Installing ISRs...\n");
	isrs_install();
	puts("Installing IRQs...\n");
	irq_install();
	puts("Setting up System Timer at 2048 Hz...\n");
	timer_phase(2048);
	timer_install();
	puts("Setting up Keyboard Controller...\n");
	keyboard_install();
	puts("Setting Interrupt Flag...\n");
	__asm__ __volatile__ ("sti"); 
	puts("Setting up serial debugging interface (COM 1)...\n\n");
	init_serial();
	printf("Testing printf: char: %c string: %s int: %i negative int: %i hex: 0x%x ", '!', "Hello world", 42, -10, 0xabcdef12);
	printf("bin: %b\n", 0b10011001);
	printf("kmain function location: 0x%x\n", &kmain);
	puts("Dumping memory map:\n");
	//unsigned char *mmap = mbi->mmap_addr; 
	//printf("mmap location: 0x%x mmap length: 0x%x\n",mbi->mmap_addr, mbi->mmap_length);
	// for (int i = 0; i < mbi->mmap_length; ++i) {
	// 	char buf[32] = {0};
	// 	itoa(*(mmap+i), buf,16);
	// 	if (buf[1] == 0) {
	// 		buf[1] = '0';
	// 		buf[2] = 0;
	// 	}
	// 	serial_puts(buf);
	// 	puts(buf);
	// 	putc(' ');
	// 	//timer_wait_ms(10);
	// }
	multiboot_memory_map_t* mmap_entry = (multiboot_memory_map_t*) mbi->mmap_addr;
	uint64 totalMem = 0;
	uint64 largestContinuousMemSize = 0;
	uint64 largestContinuousMemLocation = 0;
	while(mmap_entry < (multiboot_memory_map_t*)(mbi->mmap_addr + mbi->mmap_length)) {
		kchar type[32] = {0};
		if (mmap_entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
			strcpy(type, "MEM_AVAILABLE");
			totalMem += mmap_entry->len;
			if (mmap_entry->len > largestContinuousMemSize) {
				largestContinuousMemSize = mmap_entry->len;
				largestContinuousMemLocation = mmap_entry->addr;
			}
		} else if (mmap_entry->type == MULTIBOOT_MEMORY_RESERVED) {
			strcpy(type, "MEM_RESERVED");
		} else if (mmap_entry->type == MULTIBOOT_MEMORY_ACPI_RECLAIMABLE) {
			strcpy(type, "MEM_ACPI_RECLAIMABLE");
		} else if (mmap_entry->type == MULTIBOOT_MEMORY_NVS) {
			strcpy(type, "MEM_NVS");
		} else if (mmap_entry->type == MULTIBOOT_MEMORY_BADRAM) {
			strcpy(type, "MEM_BADRAM");
		} else {
			strcpy(type, "MEM_RESERVED_OTHER");
		}
		printf("    ENTRY: base_addr: 0x%x length: 0x%x (%i KiB) ", (uint64) mmap_entry->addr, (uint64) mmap_entry->len, (uint64) mmap_entry->len/1024);
		printf("type: %s\n", type);

		mmap_entry = (multiboot_memory_map_t*) (mmap_entry + 1);//mmap_entry->size + sizeof(mmap_entry->size));
	}
	printf("Total available memory: %l B (%l KiB / %l MiB / %l GiB)\n", totalMem, totalMem/1024, totalMem/1024/1024, totalMem/1024/1024/1024);
	printf("Longest continuous memory area: 0x%x - size: %l B (%l KiB / %l MiB / %l GiB)\n", (uint64)largestContinuousMemLocation, largestContinuousMemSize, largestContinuousMemSize/1024, largestContinuousMemSize/1024/1024, largestContinuousMemSize/1024/1024/1024);
	puts("Done\n");
	for(;;);
	return 0;
}