#include <intmath.h>
#include <memory/paging.h>
#include <memory/pmm.h>
#include <stdio.h>
#include <system.h>

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

extern void invalidateDirectory();

__attribute__((aligned(4096))) volatile paging_entry_t pml4_table[512] = {0}; // Root page table, set in CR3

__attribute__((aligned(4096))) volatile paging_entry_t pml3_table_highest[512] = {0}; // Highest PML3 table, controls the upper 512 GiB of memory

__attribute__((aligned(4096))) volatile paging_entry_t pml2_table_kernel[512] = {0}; // Controls the second highest GiB of memory. The kernel resides in the first 2 MiB of this table.
__attribute__((aligned(4096))) volatile paging_entry_t pml1_table_kernel_data[10][512] = {0}; // Each of these 10 tables control 2 MiB of memory. Part of the first table is used for the kernel, the rest is free for use
// TODO: Important! Make this dynamic



uint64 lowestKernelPML1 = UINT64_MAX;
uint64 highestKernelPML1 = 0;

#define VIRT_TO_INDICES(virtualAddress) \
((uint64)(virtualAddress) >> 39 & 0x1ff), \
((uint64)(virtualAddress) >> 30 & 0x1ff), \
((uint64)(virtualAddress) >> 21 & 0x1ff), \
((uint64)(virtualAddress) >> 12 & 0x1ff)


void mapPage(uint64 pml4_index, uint64 pml3_index, uint64 pml2_index, uint64 pml1_index, void *physicalAddress, uint64 flags) {
	assert(pml4_index < 512, "pml4_index out of bounds");
	assert(pml3_index < 512, "pml3_index out of bounds");
	assert(pml2_index < 512, "pml2_index out of bounds");
	assert(pml1_index < 512, "pml1_index out of bounds");

	if (pml4_index == 511 && pml3_index == 510 && pml2_index < 10) {

		if (pml2_index > lowestKernelPML1 && pml2_index < highestKernelPML1) { // Prevent overwriting kernel mappings
			printf("Attempted to map a page over existing kernel mappings\n");
			return;
		}

		if (!(flags & FLAG_PAGE_KERNEL) && pml1_table_kernel_data[pml2_index][pml1_index].present) { // Prevent overwriting existing mappings, except for remapping the kernel in initPaging()
			printf("Attempted to map a page over present mappings\n");
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
		
		pml1_table_kernel_data[pml2_index][pml1_index] = (paging_entry_t) {
			.present = (flags & FLAG_PAGE_PRESENT) ? 1 : 0,
			.rw = (flags & FLAG_PAGE_WRITABLE) ? 1 : 0,
			.user = (flags & FLAG_PAGE_USER) ? 1 : 0,
			.nx = (flags & FLAG_PAGE_EXECUTABLE) ? 0 : 1,
			.addr = (uint64)physicalAddress >> 12
		};
	} else {
		printf("Page Table Indices: 4: %lu, 3: %lu, 2: %lu, 1: %lu\n", pml4_index, pml3_index, pml2_index, pml1_index);
		printf("FUNCTION NOT IMPLEMENTED - NOT ACTUALLY MAPPING PAGE\n");
		// assertf("not supported yet");
	}
}


void mapRegion(void* physicalAddress, void* virtualAddress, size_t length, uint64 flags) {

	assert(length > 0, "Tried to map a region of size 0");
	assert((uint64)physicalAddress % PAGE_SIZE == 0, "Physical address is not 4KB aligned");
	assert((uint64)virtualAddress % PAGE_SIZE == 0, "Virtual address is not 4KB aligned");

	printf("Mapping %lu %s region from phys 0x%p -> virt 0x%p\n", numBytesToHuman(length), numBytesToUnit(length), physicalAddress, virtualAddress);


	if (length < PAGE_SIZE) {
		mapPage(VIRT_TO_INDICES(virtualAddress), physicalAddress, flags);			
	} else {
		uint64 numPages = intDivCeil(length, PAGE_SIZE);
		
		for (uint64 i = 0; i < numPages; i++) {
			mapPage(VIRT_TO_INDICES(virtualAddress + (i * PAGE_SIZE)), physicalAddress + (i * PAGE_SIZE), flags);
		}

	}

	invalidateDirectory();
	return;
}

uint64 upperMapLocation = (uint64)(KERNEL_OFFSET + (2*MiB)); // Start mapping things after the 2-MiB kernel page.
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

void* recursiveMappingVirtual = (void*)0xffffff0000000000;
#define GET_RECURSIVE_MAPPING(pml4, pml3, pml2, pml1) (recursiveMappingVirtual + ((pml4) << 39) + ((pml3) << 30) + ((pml2) << 21) + ((pml1) << 12))

/// @brief Replace the existing kernel mapping with 4 KiB pages using the correct protection flags
void initPaging() {
	

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
		pml1_table_kernel_data[0][i] = (paging_entry_t) {0};
	}
	for (uint64 i = highestKernelPML1 + 1; i < 512; i++) { // Remove unnecessary pages after the kernel
		pml1_table_kernel_data[0][i] = (paging_entry_t) {0};
	}
	
	for (uint64 i = 0; i < 10; i++) {
		paging_entry_t newKernelPML2Entry = {0};
		newKernelPML2Entry.present = 1;
		newKernelPML2Entry.rw = 1;
		newKernelPML2Entry.ps = 0;
		
		newKernelPML2Entry.addr = ((uint64) &pml1_table_kernel_data[i] - KERNEL_OFFSET) >> 12;

		pml2_table_kernel[i] = newKernelPML2Entry;
	}

	// Recursively map the PML4 table
	pml4_table[510].present = 1;
	pml4_table[510].rw = 1;
	pml4_table[510].ps = 0;
	pml4_table[510].addr = ((uint64) &pml4_table - KERNEL_OFFSET) >> 12;
	pml4_table[510].global = 1;
	pml4_table[510].nx = 1;

	
	invalidateDirectory();
	
}