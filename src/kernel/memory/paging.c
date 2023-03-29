#include <formatting.h>
#include <memory/paging.h>
#include <memory/pmm.h>
#include <stdio.h>
#include <system.h>
#include <string.h>

// Each PML4 entry controls 512GiB of memory, PML4 tables control 256 TiB of memory
// Each PML3 entry controls 1GiB of memory, PML3 tables control 512 GiB of memory
// Each PML2 entry controls 2MiB of memory, PML2 tables control 1 GiB of memory
// Each PML1 entry controls 4KiB of memory, PML1 tables control 2 MiB of memory

typedef struct paging_entry_t {
	uint64 present : 1; // If set, page is available 
	uint64 rw : 1; // If set, page is writable, otherwise read-only
	uint64 user : 1; // If set, page is accessible from user mode
	uint64 pwt : 1; // If set, page uses write-through caching, otherwise write-back caching is used
	uint64 pcd : 1; // If set, caching is disabled
	uint64 accessed : 1; // Enabled by the CPU if a page is accessed
	uint64 ignored : 1; // Ignored by the CPU, available for use
	uint64 ps : 1; // Reserved for PML4 entries
	uint64 global : 1; // If set, page is not flushed from the TLB when CR3 is loaded
	uint64 ignored2 : 3; // Ignored by the CPU, available for use
	uint64 addr : 40; // Address of next level page table
	uint64 ignored3 : 11; // Ignored by the CPU, available for use
	uint64 nx : 1; // If set, page is not executable
} paging_entry_t;

void reloadCR3() {
	// Reload CR3 to flush the TLB
	__asm__ volatile("movq %%cr3, %%rax; movq %%rax, %%cr3" : : : "rax");
}

void invalidateVirtualAddress(void *virtualAddress) {
	// Invalidate a page in the TLB
	__asm__ volatile("invlpg (%0)" : : "r" (virtualAddress) : "memory");
}

__attribute__((aligned(4096))) volatile paging_entry_t pml4_table[512] = {0}; // Root page table, set in CR3

__attribute__((aligned(4096))) volatile paging_entry_t pml3_table_highest[512] = {0}; // Highest PML3 table, controls the upper 512 GiB of memory

__attribute__((aligned(4096))) volatile paging_entry_t pml2_table_kernel[512] = {0}; // Controls the second highest GiB of memory. The kernel resides in the first 2 MiB of this table.

__attribute__((aligned(4096))) volatile paging_entry_t pml1_table_kernel[512] = {0}; // Controls the first 2 MiB of the second-highest GiB of memory. This table controls access to the kernel.



uint64 lowestKernelPML1 = UINT64_MAX;
uint64 highestKernelPML1 = 0;

#define VIRT_TO_INDICES(virtualAddress) \
((uint64)(virtualAddress) >> 39 & 0x1ff), \
((uint64)(virtualAddress) >> 30 & 0x1ff), \
((uint64)(virtualAddress) >> 21 & 0x1ff), \
((uint64)(virtualAddress) >> 12 & 0x1ff)

#define RECURSIVE_ADDR 0xffffff0000000000 // We create a recursive mapping in pml4[510]
#define REC_POS 510


// Gets the page entry that controls a virtual address by using the recursive mapping
paging_entry_t * indicesToVirtRecursive(uint64 p3, uint64 p2, uint64 p1, uint64 p0) {
	return (paging_entry_t*)(RECURSIVE_ADDR + (((uint64)p3) << 30) + (((uint64)p2) << 21) + (((uint64)p1) << 12) + (((uint64)p0) << 3));
}

void mapPage(uint64 pml4_index, uint64 pml3_index, uint64 pml2_index, uint64 pml1_index, void *physicalAddress, uint64 flags) {
	assert(pml4_index < 512, "pml4_index out of bounds");
	assert(pml3_index < 512, "pml3_index out of bounds");
	assert(pml2_index < 512, "pml2_index out of bounds");
	assert(pml1_index < 512, "pml1_index out of bounds");

	paging_entry_t *pml4_check = indicesToVirtRecursive(REC_POS, REC_POS, REC_POS, pml4_index);
	if (!pml4_check->present) {
		printf("PML4 entry (%ld, %ld, %ld, %ld) not present, creating new table\n", pml4_index, pml3_index, pml2_index, pml1_index);
		uint64 newTable = (uint64)getFreePhysicalPage();
		printf("New table at %lx\n", newTable);
		*pml4_check = (paging_entry_t) {
			.present = 1,
			.rw = 1,
			.user = 0,
			.nx = 1,
			.addr = newTable >> 12
		};
		invalidateVirtualAddress(pml4_check);
		
		// Clear the new table
		memset(indicesToVirtRecursive(REC_POS, REC_POS, pml4_index, 0), 0, 4096);\
	}

	paging_entry_t *pml3_check = indicesToVirtRecursive(REC_POS, REC_POS, pml4_index, pml3_index);
	if (!pml3_check->present) {
		printf("PML3 entry (%ld, %ld, %ld, %ld) not present, creating new table\n", pml4_index, pml3_index, pml2_index, pml1_index);
		uint64 newTable = (uint64)getFreePhysicalPage();
		printf("New table at %lx\n", newTable);
		*pml3_check = (paging_entry_t) {
			.present = 1,
			.rw = 1,
			.user = 0,
			.nx = 1,
			.addr = newTable >> 12
		};
		invalidateVirtualAddress(pml3_check);

		// Clear the new table
		memset(indicesToVirtRecursive(REC_POS, pml4_index, pml3_index, 0), 0, 4096);
	}

	paging_entry_t *pml2_check = indicesToVirtRecursive(REC_POS, pml4_index, pml3_index, pml2_index);
	if (!pml2_check->present) {
		printf("PML2 entry (%ld, %ld, %ld, %ld) not present, creating new table\n", pml4_index, pml3_index, pml2_index, pml1_index);
		uint64 newTable = (uint64)getFreePhysicalPage();
		printf("New table at %lx\n", newTable);
		*pml2_check = (paging_entry_t) {
			.present = 1,
			.rw = 1,
			.user = 0,
			.nx = 1,
			.addr = newTable >> 12
		};
		invalidateVirtualAddress(pml2_check);
		
		// Clear the new table
		memset(indicesToVirtRecursive(pml4_index, pml3_index, pml2_index, 0), 0, 4096);
	}

	paging_entry_t *pml1_check = indicesToVirtRecursive(pml4_index, pml3_index, pml2_index, pml1_index);
	if (pml1_check->present && !(flags & FLAG_PAGE_KERNEL)) {
		printf("Page (%ld %ld %ld %ld) already mapped!\n", pml4_index, pml3_index, pml2_index, pml1_index);
		return;
	}

	if ((flags & FLAG_PAGE_KERNEL) && pml4_index == 511 && pml3_index == 510 && pml2_index == 0) {
		if (pml1_index < lowestKernelPML1) {
			lowestKernelPML1 = pml1_index;
		}
		if (pml1_index > highestKernelPML1) {
			highestKernelPML1 = pml1_index;
		}
	}
	
	*pml1_check = (paging_entry_t) {
		.present = (flags & FLAG_PAGE_PRESENT) ? 1 : 0,
		.rw = (flags & FLAG_PAGE_WRITABLE) ? 1 : 0,
		.user = (flags & FLAG_PAGE_USER) ? 1 : 0,
		.nx = (flags & FLAG_PAGE_EXECUTABLE) ? 0 : 1,
		.addr = (uint64)physicalAddress >> 12
	};
	invalidateVirtualAddress(pml1_check);
}


void mapRegion(void* physicalAddress, void* virtualAddress, size_t length, uint64 flags) {

	assert(length > 0, "Tried to map a region of size 0");
	assert((uint64)physicalAddress % PAGE_SIZE == 0, "Physical address is not 4KB aligned");
	assert((uint64)virtualAddress % PAGE_SIZE == 0, "Virtual address is not 4KB aligned");

	uint64 numPages = intDivCeil(length, PAGE_SIZE);

	printf("Mapping %lu %s region from phys 0x%p -> virt 0x%p (%lu %i-B page(s))\n", numBytesToHuman(length), numBytesToUnit(length), physicalAddress, virtualAddress, numPages, PAGE_SIZE);


	if (numPages == 1) {
		mapPage(VIRT_TO_INDICES(virtualAddress), physicalAddress, flags);			
	} else {
		for (uint64 i = 0; i < numPages; i++) {
			mapPage(VIRT_TO_INDICES(virtualAddress + (i * PAGE_SIZE)), physicalAddress + (i * PAGE_SIZE), flags);
		}
	}

	return;
}


// We start mapping anything the kernel needs right after the memory gap
uint64 upperMapLocation = 0xffff800000000000;


// TODO: Change this to actually keep track of used and freed virtual memory


/// @brief Maps a region of physical memory to kernel virtual memory
/// @param physicalAddress The physical address to be mapped
/// @param length The length of the region to be mapped
/// @param flags Allows to configure how the page is mapped
/// @return A pointer to the virtual address of the mapped region
void* mapPhysicalToKernel(void* physicalAddress, size_t length, uint64 flags) {

	assert(length > 0, "Tried to map a region of size 0");
	assert(physicalAddress != NULL, "Tried to map a region with a NULL physical address");
	assert((uint64)physicalAddress % PAGE_SIZE == 0, "Physical address is not 4KB aligned");

	if (upperMapLocation % PAGE_SIZE != 0) {
		upperMapLocation += PAGE_SIZE - (upperMapLocation % PAGE_SIZE);
	}

	void* virtualAddress = (void *)upperMapLocation;

	mapRegion(physicalAddress, virtualAddress, length, flags);

	upperMapLocation += length;

	return virtualAddress;
}

/// @brief Replace the existing kernel mapping with 4 KiB pages using the correct protection flags
void initPaging() {

	// Recursively map the PML4 table
	pml4_table[REC_POS].present = 1;
	pml4_table[REC_POS].rw = 1;
	pml4_table[REC_POS].ps = 0;
	pml4_table[REC_POS].addr = ((uint64) &pml4_table[510] - KERNEL_OFFSET) >> 12;
	pml4_table[REC_POS].global = 1;
	pml4_table[REC_POS].nx = 1;

	reloadCR3();
	

	printf("Kernel text   start: 0x%p, end: 0x%p, size: %lu %s\n", TEXT_START, TEXT_END, numBytesToHuman(TEXT_SIZE), numBytesToUnit(TEXT_SIZE));
	printf("Kernel rodata start: 0x%p, end: 0x%p, size: %lu %s\n", RODATA_START, RODATA_END, numBytesToHuman(RODATA_SIZE), numBytesToUnit(RODATA_SIZE));
	printf("Kernel data   start: 0x%p, end: 0x%p, size: %lu %s\n", DATA_START, DATA_END, numBytesToHuman(DATA_SIZE), numBytesToUnit(DATA_SIZE));
	printf("Kernel bss    start: 0x%p, end: 0x%p, size: %lu %s\n", BSS_START, BSS_END, numBytesToHuman(BSS_SIZE), numBytesToUnit(BSS_SIZE));

	for (void* addr = TEXT_START; addr < TEXT_END; addr+=PAGE_SIZE) {
		// printf("[ TEXT ] phys addr: 0x%p, index: %lu\n", addr-KERNEL_OFFSET, ((uint64)addr-KERNEL_OFFSET) / PAGE_SIZE);
		mapPage(VIRT_TO_INDICES(addr), addr - KERNEL_OFFSET, FLAG_PAGE_KERNEL | FLAG_PAGE_PRESENT | FLAG_PAGE_EXECUTABLE); // Map kernel text as executable and read-only
	}

	for (void* addr = RODATA_START; addr < RODATA_END; addr+=PAGE_SIZE) {
		// printf("[RODATA] phys addr: 0x%p, index: %lu\n", addr-KERNEL_OFFSET, ((uint64)addr-KERNEL_OFFSET) / PAGE_SIZE);
		mapPage(VIRT_TO_INDICES(addr), addr - KERNEL_OFFSET, FLAG_PAGE_KERNEL | FLAG_PAGE_PRESENT); // Map kernel rodata as read-only
	}

	for (void* addr = DATA_START; addr < DATA_END; addr+=PAGE_SIZE) {
		// printf("[ DATA ] phys addr: 0x%p, index: %lu\n", addr-KERNEL_OFFSET, ((uint64)addr-KERNEL_OFFSET) / PAGE_SIZE);
		mapPage(VIRT_TO_INDICES(addr), addr - KERNEL_OFFSET, FLAG_PAGE_KERNEL | FLAG_PAGE_PRESENT | FLAG_PAGE_WRITABLE); // Map kernel data as read-write
	}

	for (void* addr = BSS_START; addr < BSS_END; addr+=PAGE_SIZE) {
		// printf("[ BSS  ] phys addr: 0x%p, index: %lu\n", addr-KERNEL_OFFSET, ((uint64)addr-KERNEL_OFFSET) / PAGE_SIZE);
		mapPage(VIRT_TO_INDICES(addr), addr - KERNEL_OFFSET, FLAG_PAGE_KERNEL | FLAG_PAGE_PRESENT | FLAG_PAGE_WRITABLE); // Map kernel bss as read-write
	}

	printf("First kernel page index: %lu, last kernel page index: %lu\n", lowestKernelPML1, highestKernelPML1);

	for (uint64 i = 0; i < lowestKernelPML1; i++) { // Remove unnecessary pages before the kernel
		*indicesToVirtRecursive(511, 510, 0, i) = (paging_entry_t) {0};
	}
	for (uint64 i = highestKernelPML1 + 1; i < 512; i++) { // Remove unnecessary pages after the kernel
		*indicesToVirtRecursive(511, 510, 0, i) = (paging_entry_t) {0};
	}
	
	reloadCR3();

	if (upperMapLocation % PAGE_SIZE != 0) {
		upperMapLocation += PAGE_SIZE - (upperMapLocation % PAGE_SIZE);
	}
}
