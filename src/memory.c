#include <header.h>
#include <multiboot.h>

void install_memory(uint32 mmap_addr, uint32 mmap_length, uint32 *kmain) {
	multiboot_memory_map_t* mmap_entry = (multiboot_memory_map_t*) mmap_addr;

	extern uint64 __END_OF_KERNEL;
	uint64 endOfKernel = (uint32) &__END_OF_KERNEL;
	extern uint64 __START_OF_KERNEL;
	uint64 startOfKernel = (uint32) &__START_OF_KERNEL;
	uint64 sizeOfKernel = (uint64)endOfKernel - (uint64)startOfKernel;

	uint64 totalMem = 0;
	uint64 largestContinuousMemSize = 0;
	uint64 largestContinuousMemLocation = 0;

	puts("Dumping memory map:\n");
	while(mmap_entry < (multiboot_memory_map_t*)(mmap_addr + mmap_length)) {
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
			strcpy(type, "MEM_RESERVED_UNKNOWN");
		}
		printf("    ENTRY: base_addr: 0x%x length: 0x%x (%i ",
			mmap_entry->addr,
			mmap_entry->len, 
			mmap_entry->len/1024 > 10240 ? mmap_entry->len/1024/1024 : mmap_entry->len/1024);
		printf("%s) type: %s\n",
			mmap_entry->len/1024 > 10240 ? "MiB" : "KiB",
			type);

		mmap_entry = (multiboot_memory_map_t*) (mmap_entry + 1);
	}
	putc('\n');
	printf("Total available memory: %l B (%l KiB / %l MiB / %l GiB)\n", totalMem, totalMem/1024, totalMem/1024/1024, totalMem/1024/1024/1024);
	printf("Longest continuous memory area: 0x%x - size: %l B (%l KiB / %l MiB / %l GiB)\n", largestContinuousMemLocation, largestContinuousMemSize, largestContinuousMemSize/1024, largestContinuousMemSize/1024/1024, largestContinuousMemSize/1024/1024/1024);
	printf("kmain function location: 0x%x\n", &kmain);
	printf("Kernel start: 0x%x end: 0x%x len: 0x%x (%i B)\n", (uint64)startOfKernel, (uint64)endOfKernel, sizeOfKernel, sizeOfKernel);
	
	printf("Kernel is loaded at start of largest memory area: %B\n", largestContinuousMemLocation == startOfKernel);
	if (largestContinuousMemLocation == startOfKernel) {
		printf("Start of free memory: 0x%x\n", endOfKernel);
	}
}

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