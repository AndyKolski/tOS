#include <display.h>
#include <libs.h>
#include <memory.h>
#include <multiboot.h>
#include <stdio.h>
#include <string.h>
#include <system.h>

void* freememStart;
void* freememEnd;

uint32 firstUsablePage = 0;

typedef struct HeapManagerEntry {
	uint32 inUse		: 1;
	uint32 isHead		: 1;
	uint32 isUsable		: 1;
	// uint32 attr		: 1;
	// uint32 attr		: 1;
	// uint32 attr		: 1;
	// uint32 attr		: 1;
	// uint32 attr		: 1;
	// uint32 attr		: 1;
	// uint32 attr		: 1;
	// uint32 attr		: 1;
	// uint32 attr		: 1;
	uint32 length		: 20;
} HeapManagerEntry;

C_ASSERT(sizeof(HeapManagerEntry) == 4);

#define PAGE_SIZE 4096
#define HEAP_MANAGER_ARRAY_SIZE 1048576 // 1048576 = 2^32 / PAGE_SIZE = 2^20

HeapManagerEntry (*heapMannagerArray)[HEAP_MANAGER_ARRAY_SIZE];

HeapManagerEntry createEntry(uint32 length, uint32 inUse, uint32 isHead, uint32 isUsable) {
	HeapManagerEntry new;
	new.length = length;
	new.inUse = inUse;
	new.isHead = isHead;
	new.isUsable = isUsable;
	return new;
}

void install_memory(multiboot_memory_map_t* mmap_addr, uint32 mmap_length) {
	multiboot_memory_map_t* mmap_entry = mmap_addr;

	extern void* KERNEL_START;
	extern void* KERNEL_END;
	void* startOfKernel = &KERNEL_START; 
	void* endOfKernel = &KERNEL_END;
	uint64 sizeOfKernel = (uint64)(endOfKernel - startOfKernel);

	uint64 totalMem = 0;
	uint64 largestContinuousMemSize = 0;
	void* largestContinuousMemLocation = 0;

	// puts("Memory map:");
	while(mmap_entry < (multiboot_memory_map_t*)((void*)mmap_addr + mmap_length)) {
		kchar type[75] = {0};
		if (mmap_entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
			strcpy(type, "MEM_AVAILABLE");
			totalMem += mmap_entry->len;
			if (mmap_entry->addr + mmap_entry->len > 0xffffffff) {
				strcat(type, " - past 32 bits, not marking as available");
			} else {
				if (mmap_entry->len > largestContinuousMemSize) {
					largestContinuousMemSize = mmap_entry->len;
					largestContinuousMemLocation = (void*)(uint32)mmap_entry->addr;
				}
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
		// printf("    ENTRY: address: 0x%08qx length: 0x%08qx (%5qu %s) type: %s\n",
		// 	mmap_entry->addr,
		// 	mmap_entry->len,
		// 	mmap_entry->len/1024 > 10240 ? (mmap_entry->len/1024/1024) : (mmap_entry->len/1024),
		// 	mmap_entry->len/1024 > 10240 ? "MiB" : "KiB",
		// 	type);
		mmap_entry = (multiboot_memory_map_t*) (mmap_entry + 1);
	}
	printf("Total available memory: %qu B (%qu KiB / %qu MiB / %qu GiB)\n", totalMem, (totalMem/1024), ((totalMem/1024+10)/1024), ((totalMem/1024/1024+10)/1024));
	printf("Longest continuous memory area: 0x%08qx - size: %qu B (%qu KiB / %qu MiB / %qu GiB)\n", (uint64)(uint32)largestContinuousMemLocation, largestContinuousMemSize, (largestContinuousMemSize/1024), ((largestContinuousMemSize/1024+10)/1024), ((largestContinuousMemSize/1024/1024+10)/1024));
	printf("Kernel start: 0x%08qx end: 0x%08qx len: 0x%qx (%qu KiB) loaded at start of largest memory area: %s\n", (uint64)(uint32)startOfKernel, (uint64)(uint32)endOfKernel, sizeOfKernel, (sizeOfKernel/1024), largestContinuousMemLocation == startOfKernel ? "true" : "false");

	if (largestContinuousMemLocation == startOfKernel) {
		heapMannagerArray = endOfKernel;
		freememStart = endOfKernel + sizeof(*heapMannagerArray);
		freememEnd = largestContinuousMemLocation + largestContinuousMemSize - sizeOfKernel - sizeof(*heapMannagerArray);
	} else {
		heapMannagerArray = largestContinuousMemLocation;
		freememStart = largestContinuousMemLocation + sizeof(*heapMannagerArray);
		freememEnd = largestContinuousMemLocation + largestContinuousMemSize - sizeof(*heapMannagerArray);
	}

	// TODO: Use more than just the continuous memory starting at 0x00100000

	for (uint32 i = 0; i < HEAP_MANAGER_ARRAY_SIZE; ++i) {
		bool isUsable = false;
		if ((void*)(i * PAGE_SIZE) >= freememStart && (void*)(((i+1) * PAGE_SIZE)-1) <= freememEnd) {
			isUsable = true;
			if (firstUsablePage == 0) {
				firstUsablePage = i;
			}
		}
		(*heapMannagerArray)[i] = createEntry(0, 0, 0, isUsable);
	}
	printf("Start of free memory: 0x%08lx, %lu MiB free\n", (uint32)freememStart, bytesFree()/1024/1024);
}

uint32 intdivceil(uint32 a, uint32 b) { // calculates ceil(a/b) without using any floating point math
	if (a%b == 0) {
		return a/b;
	} else {
		return a/b + 1;
	}
}

uint32 bytesFree() {
	uint32 freeBytes = 0;
	for (uint32 i = firstUsablePage; i < HEAP_MANAGER_ARRAY_SIZE-1; ++i) {
		HeapManagerEntry current = (*heapMannagerArray)[i];
		if (current.isUsable && !current.inUse) {
			freeBytes += PAGE_SIZE;
		}
	}
	return freeBytes;
}

void *kmalloc(size_t size) {
	uint32 pagesRequired = intdivceil(size, PAGE_SIZE);
	for (uint32 i = firstUsablePage; i < HEAP_MANAGER_ARRAY_SIZE-1; ++i) {
		HeapManagerEntry current = (*heapMannagerArray)[i];
		if (current.isUsable && !current.inUse) {
			if (pagesRequired == 1) {
				current.inUse = true;
				current.isHead = true;
				current.length = pagesRequired;
				(*heapMannagerArray)[i] = current;
				return (void*) (i*PAGE_SIZE);
			} else {
				bool cantUse = false;
				for (uint32 o = 1; o < pagesRequired; ++o) {
					HeapManagerEntry test = (*heapMannagerArray)[i+o];
					if (!test.isUsable || test.inUse) {
						cantUse = true;
					}
				}
				if (!cantUse) {
					for (uint32 o = 1; o < pagesRequired; ++o) {
						HeapManagerEntry test = (*heapMannagerArray)[i+o];
						test.inUse = true;
						test.isHead = false;
						test.length = 0;
						(*heapMannagerArray)[i+o] = test;
					}
					current.inUse = true;
					current.isHead = true;
					current.length = pagesRequired;
					(*heapMannagerArray)[i] = current;
					return (void*) (i*PAGE_SIZE);
				}
			}
		}
	}
	
	// TODO: Implement OOM killer & memory defragmenter at some point.

	panic("Out of memory!");
	return 0;
}

void kfree(void *ptr) {
	assert((uintptr_t)ptr%PAGE_SIZE == 0, "Attempt to kfree() invalid pointer!");
	
	if ((uintptr_t)ptr == 0) {
		return;
	}

	uint32 headIndex = (uintptr_t)ptr/PAGE_SIZE;
	HeapManagerEntry current = (*heapMannagerArray)[headIndex];
	
	if(!current.inUse || !current.isHead) {
		assertf("Attempt to kfree() invalid pointer!");
	}

	if (current.length == 1) {
		current.inUse = false;
		current.isHead = false;
		current.length = 0;
		(*heapMannagerArray)[headIndex] = current;
		return;
	} else {
		for (uint32 i = 1; i < current.length; ++i) {
			HeapManagerEntry test = (*heapMannagerArray)[headIndex+i];
			test.inUse = false;
			test.isHead = false;
			test.length = 0;
			(*heapMannagerArray)[headIndex+i] = test;
		}
		current.inUse = false;
		current.isHead = false;
		current.length = 0;
		(*heapMannagerArray)[headIndex] = current;
		return;
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