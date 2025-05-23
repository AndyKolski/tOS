#include <formatting.h>
#include <memory/memregion.h>
#include <memory/paging.h>
#include <memory/pmm.h>
#include <stdio.h>
#include <string.h>
#include <system.h>

// Each PML4 entry controls 512GiB of memory, PML4 tables control 256 TiB of memory
// Each PML3 entry controls 1GiB of memory, PML3 tables control 512 GiB of memory
// Each PML2 entry controls 2MiB of memory, PML2 tables control 1 GiB of memory
// Each PML1 entry controls 4KiB of memory, PML1 tables control 2 MiB of memory

/// @brief Reloads CR3 register to flush the TLB. Global pages are not flushed.
void reloadCR3() {
	asm volatile("movq %%cr3, %%rax; movq %%rax, %%cr3" : : : "rax");
}

/// @brief Invalidates a single page in the TLB.
/// @param virtualAddress The virtual address to invalidate.
void invalidateVirtualAddress(void* virtualAddress) {
	asm volatile("invlpg (%0)" : : "r"(virtualAddress) : "memory");
}

__attribute__((aligned(PAGE_SIZE))) volatile paging_entry_t pml4_table[512] = {0}; // Root page table, set in CR3

__attribute__((aligned(PAGE_SIZE))) volatile paging_entry_t pml3_table_highest[512] = {0}; // Highest PML3 table, controls the upper 512 GiB of memory

__attribute__((aligned(PAGE_SIZE))) volatile paging_entry_t pml2_table_kernel[512] = {0}; // Controls the second highest GiB of memory. The kernel resides in the first 2 MiB of this table.

__attribute__((aligned(PAGE_SIZE))) volatile paging_entry_t pml1_table_kernel[512] = {0}; // Controls the first 2 MiB of the second-highest GiB of memory. This table controls access to the kernel.

uint64 lowestKernelPML1 = UINT64_MAX;
uint64 highestKernelPML1 = 0;

#define VIRT_TO_INDICES(virtualAddress) ((uint64)(virtualAddress) >> 39 & 0x1ff), \
										((uint64)(virtualAddress) >> 30 & 0x1ff), \
										((uint64)(virtualAddress) >> 21 & 0x1ff), \
										((uint64)(virtualAddress) >> 12 & 0x1ff)

#define REC_POS        510
#define RECURSIVE_ADDR 0xffffff0000000000 // We create a recursive mapping in pml4[REC_POS]

void getIndicesFromVirtualAddress(void* virtualAddress, uint64* pml4_index, uint64* pml3_index, uint64* pml2_index, uint64* pml1_index) {
	uint64 addr = (uint64)virtualAddress;
	*pml4_index = (addr >> 39) & 0x1ff;
	*pml3_index = (addr >> 30) & 0x1ff;
	*pml2_index = (addr >> 21) & 0x1ff;
	*pml1_index = (addr >> 12) & 0x1ff;
}

// Gets the page entry that controls a virtual address by using the recursive mapping
paging_entry_t* indicesToVirtRecursive(uint64 p3, uint64 p2, uint64 p1, uint64 p0) {
	return (paging_entry_t*)(RECURSIVE_ADDR + (((uint64)p3) << 30) + (((uint64)p2) << 21) + (((uint64)p1) << 12) + (((uint64)p0) << 3));
}

void mapPage(uint64 pml4_index, uint64 pml3_index, uint64 pml2_index, uint64 pml1_index, void* physicalAddress, uint64 flags) {
	assert(pml4_index < 512, "pml4_index out of bounds");
	assert(pml3_index < 512, "pml3_index out of bounds");
	assert(pml2_index < 512, "pml2_index out of bounds");
	assert(pml1_index < 512, "pml1_index out of bounds");

	paging_entry_t* pml4_check = indicesToVirtRecursive(REC_POS, REC_POS, REC_POS, pml4_index);
	if (!pml4_check->present) {
		DEBUG(printf("PML4 entry (%ld, %ld, %ld, %ld) not present, creating new table\n", pml4_index, pml3_index, pml2_index, pml1_index););
		uint64 newTable = (uint64)getPhysicalPage();
		DEBUG(printf("New table at %lx\n", newTable););
		*pml4_check = (paging_entry_t){.present = 1, .rw = 1, .user = 0, .nx = 1, .addr = newTable >> 12};
		invalidateVirtualAddress(pml4_check);

		// Clear the new table
		memset(indicesToVirtRecursive(REC_POS, REC_POS, pml4_index, 0), 0, PAGE_SIZE);
	}

	paging_entry_t* pml3_check = indicesToVirtRecursive(REC_POS, REC_POS, pml4_index, pml3_index);
	if (!pml3_check->present) {
		DEBUG(printf("PML3 entry (%ld, %ld, %ld, %ld) not present, creating new table\n", pml4_index, pml3_index, pml2_index, pml1_index););
		uint64 newTable = (uint64)getPhysicalPage();
		DEBUG(printf("New table at %lx\n", newTable););
		*pml3_check = (paging_entry_t){.present = 1, .rw = 1, .user = 0, .nx = 1, .addr = newTable >> 12};
		invalidateVirtualAddress(pml3_check);

		// Clear the new table
		memset(indicesToVirtRecursive(REC_POS, pml4_index, pml3_index, 0), 0, PAGE_SIZE);
	}

	paging_entry_t* pml2_check = indicesToVirtRecursive(REC_POS, pml4_index, pml3_index, pml2_index);
	if (!pml2_check->present) {
		DEBUG(printf("PML2 entry (%ld, %ld, %ld, %ld) not present, creating new table\n", pml4_index, pml3_index, pml2_index, pml1_index););
		uint64 newTable = (uint64)getPhysicalPage();
		DEBUG(printf("New table at %lx\n", newTable););
		*pml2_check = (paging_entry_t){.present = 1, .rw = 1, .user = 0, .nx = 1, .addr = newTable >> 12};
		invalidateVirtualAddress(pml2_check);

		// Clear the new table
		memset(indicesToVirtRecursive(pml4_index, pml3_index, pml2_index, 0), 0, PAGE_SIZE);
	}

	paging_entry_t* pml1_check = indicesToVirtRecursive(pml4_index, pml3_index, pml2_index, pml1_index);
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

	*pml1_check = (paging_entry_t){.present = (flags & FLAG_PAGE_PRESENT) ? 1 : 0,
	                               .rw = (flags & FLAG_PAGE_WRITABLE) ? 1 : 0,
	                               .user = (flags & FLAG_PAGE_USER) ? 1 : 0,
	                               .nx = (flags & FLAG_PAGE_EXECUTABLE) ? 0 : 1,
	                               .pwt = (flags & FLAG_PAGE_WRITETHROUGH_CACHE) ? 1 : 0,
	                               .pcd = (flags & FLAG_PAGE_CACHE_DISABLE) ? 1 : 0,
	                               .global = (flags & FLAG_PAGE_GLOBAL) ? 1 : 0,
	                               .addr = (uint64)physicalAddress >> 12};
	invalidateVirtualAddress(pml1_check);
}

void* getVirtualAddressInfo(void* virtualAddress) {
	printf("Info for virtual address 0x%p:\n", virtualAddress);

	uint64 pml4_index;
	uint64 pml3_index;
	uint64 pml2_index;
	uint64 pml1_index;
	getIndicesFromVirtualAddress(virtualAddress, &pml4_index, &pml3_index, &pml2_index, &pml1_index);

	paging_entry_t* pml4_check = indicesToVirtRecursive(REC_POS, REC_POS, REC_POS, pml4_index);
	if (!pml4_check->present) {
		printf("PML4 entry (%d, %d, %d, %ld) not present\n", REC_POS, REC_POS, REC_POS, pml4_index);
		return NULL;
	}

	paging_entry_t* pml3_check = indicesToVirtRecursive(REC_POS, REC_POS, pml4_index, pml3_index);
	if (!pml3_check->present) {
		printf("PML3 entry (%d, %d, %ld, %ld) not present\n", REC_POS, REC_POS, pml4_index, pml3_index);
		return NULL;
	}

	paging_entry_t* pml2_check = indicesToVirtRecursive(REC_POS, pml4_index, pml3_index, pml2_index);
	if (!pml2_check->present) {
		printf("PML2 entry (%d, %ld, %ld, %ld) not present\n", REC_POS, pml4_index, pml3_index, pml2_index);
		return NULL;
	}

	paging_entry_t* pml1_check = indicesToVirtRecursive(pml4_index, pml3_index, pml2_index, pml1_index);
	if (!pml1_check->present) {
		printf("PML1 entry (%ld, %ld, %ld, %ld) not present\n", pml4_index, pml3_index, pml2_index, pml1_index);
		return NULL;
	}

	void* physAddr = (void*)(uintptr_t)(pml1_check->addr << 12);

	printf("Virtual address is mapped to physical address 0x%p\n", physAddr);
	printf("Managed by recursive mapping (%ld %ld %ld %ld)\n", pml4_index, pml3_index, pml2_index, pml1_index);

	return physAddr;
}

/// @brief Maps a region of physical memory to a region of virtual memory
/// @param physicalRegion The region of physical memory to map
/// @param virtualRegion  The region of virtual memory to map to
/// @param flags A bitmap of flags to control the mapping behaviour and change page options
void mapRegion(memregion_t physicalRegion, memregion_t virtualRegion, uint64 flags) {
	assert(physicalRegion.length == virtualRegion.length, "Physical and virtual regions are not the same size");
	assert(physicalRegion.length > 0, "Tried to map a region of size 0");
	assert((uint64)physicalRegion.start % PAGE_SIZE == 0, "Physical address is not 4KB aligned");
	assert((uint64)virtualRegion.start % PAGE_SIZE == 0, "Virtual address is not 4KB aligned");
	assert(physicalRegion.isVirtual == false, "Physical region is virtual");
	assert(virtualRegion.isVirtual == true, "Virtual region is not virtual");

	uint64 numPages = intDivCeil(physicalRegion.length, PAGE_SIZE);

	DEBUG(printf("Mapping %lu %s region from phys 0x%p-0x%p -> virt 0x%p-0x%p (%lu %lu-%s page(s))\n", numBytesToHuman(physicalRegion.length), numBytesToUnit(physicalRegion.length),
	             physicalRegion.start, physicalRegion.start + physicalRegion.length, virtualRegion.start, virtualRegion.start + virtualRegion.length, numPages, numBytesToHuman(PAGE_SIZE),
	             numBytesToUnit(PAGE_SIZE)););

	if (numPages == 1) {
		mapPage(VIRT_TO_INDICES(virtualRegion.start), physicalRegion.start, flags);
	} else {
		for (uint64 i = 0; i < numPages; i++) {
			mapPage(VIRT_TO_INDICES(virtualRegion.start + (i * PAGE_SIZE)), physicalRegion.start + (i * PAGE_SIZE), flags);
		}
	}

	return;
}

// We start mapping anything the kernel needs right after the memory gap
uint64 upperMapLocation = 0xffff800000000000;

// TODO: Change this to actually keep track of used and freed virtual memory

/// @brief Maps a region of physical memory to kernel virtual memory
/// @param physicalRegion The region of physical memory to map
/// @param flags Allows to configure how the page is mapped
/// @return The virtual region that the physical region was mapped to
memregion_t mapPhysicalToKernel(memregion_t physicalRegion, uint64 flags) {
	assert(physicalRegion.length > 0, "Tried to map a region of size 0");
	assert(physicalRegion.start != NULL, "Tried to map a region with a NULL physical address");
	assert((uint64)physicalRegion.start % PAGE_SIZE == 0, "Physical address is not 4KB aligned");
	assert(physicalRegion.isVirtual == false, "Physical region is virtual");

	if (upperMapLocation % PAGE_SIZE != 0) {
		upperMapLocation += PAGE_SIZE - (upperMapLocation % PAGE_SIZE);
	}

	memregion_t virtualRegion = createMemRegion((void*)upperMapLocation, physicalRegion.length, true);
	upperMapLocation += physicalRegion.length;

	mapRegion(physicalRegion, virtualRegion, flags);

	return virtualRegion;
}

/// @brief Sets up recursive paging and replaces the existing kernel mapping with one which uses 4 KiB pages and has the correct protection flags
void initPaging() {
	// Recursively map the PML4 table
	pml4_table[REC_POS].present = 1;
	pml4_table[REC_POS].rw = 1;
	pml4_table[REC_POS].ps = 0;
	pml4_table[REC_POS].addr = ((uint64)&pml4_table[510] - KERNEL_OFFSET) >> 12;
	pml4_table[REC_POS].global = 1;
	pml4_table[REC_POS].nx = 1;

	reloadCR3();

	printf("Kernel physical start: 0x%08lx end: 0x%08lx len: 0x%lx (%lu %s)\n", (intptr_t)KERNEL_START, (intptr_t)KERNEL_END, KERNEL_SIZE, numBytesToHuman(KERNEL_SIZE), numBytesToUnit(KERNEL_SIZE));

	printf("Offset: 0x%lx\n", KERNEL_OFFSET);

	printf("Kernel text   start: 0x%p, end: 0x%p, size: %lu %s\n", TEXT_START, TEXT_END, numBytesToHuman(TEXT_SIZE), numBytesToUnit(TEXT_SIZE));
	printf("Kernel rodata start: 0x%p, end: 0x%p, size: %lu %s\n", RODATA_START, RODATA_END, numBytesToHuman(RODATA_SIZE), numBytesToUnit(RODATA_SIZE));
	printf("Kernel data   start: 0x%p, end: 0x%p, size: %lu %s\n", DATA_START, DATA_END, numBytesToHuman(DATA_SIZE), numBytesToUnit(DATA_SIZE));
	printf("Kernel bss    start: 0x%p, end: 0x%p, size: %lu %s\n", BSS_START, BSS_END, numBytesToHuman(BSS_SIZE), numBytesToUnit(BSS_SIZE));

	for (void* addr = TEXT_START; addr < TEXT_END; addr += PAGE_SIZE) {
		DEBUG(printf("[ TEXT ] phys addr: 0x%p, index: %lu\n", addr - KERNEL_OFFSET, ((uint64)addr - KERNEL_OFFSET) / PAGE_SIZE););
		mapPage(VIRT_TO_INDICES(addr), addr - KERNEL_OFFSET, FLAG_PAGE_KERNEL | FLAG_PAGE_GLOBAL | FLAG_PAGE_PRESENT | FLAG_PAGE_EXECUTABLE); // Map kernel text as executable and read-only
	}

	for (void* addr = RODATA_START; addr < RODATA_END; addr += PAGE_SIZE) {
		DEBUG(printf("[RODATA] phys addr: 0x%p, index: %lu\n", addr - KERNEL_OFFSET, ((uint64)addr - KERNEL_OFFSET) / PAGE_SIZE););
		mapPage(VIRT_TO_INDICES(addr), addr - KERNEL_OFFSET, FLAG_PAGE_KERNEL | FLAG_PAGE_GLOBAL | FLAG_PAGE_PRESENT); // Map kernel rodata as read-only
	}

	for (void* addr = DATA_START; addr < DATA_END; addr += PAGE_SIZE) {
		DEBUG(printf("[ DATA ] phys addr: 0x%p, index: %lu\n", addr - KERNEL_OFFSET, ((uint64)addr - KERNEL_OFFSET) / PAGE_SIZE););
		mapPage(VIRT_TO_INDICES(addr), addr - KERNEL_OFFSET, FLAG_PAGE_KERNEL | FLAG_PAGE_GLOBAL | FLAG_PAGE_PRESENT | FLAG_PAGE_WRITABLE); // Map kernel data as read-write
	}

	for (void* addr = BSS_START; addr < BSS_END; addr += PAGE_SIZE) {
		DEBUG(printf("[ BSS  ] phys addr: 0x%p, index: %lu\n", addr - KERNEL_OFFSET, ((uint64)addr - KERNEL_OFFSET) / PAGE_SIZE););
		mapPage(VIRT_TO_INDICES(addr), addr - KERNEL_OFFSET, FLAG_PAGE_KERNEL | FLAG_PAGE_GLOBAL | FLAG_PAGE_PRESENT | FLAG_PAGE_WRITABLE); // Map kernel bss as read-write
	}

	printf("First kernel page index: %lu, last kernel page index: %lu\n", lowestKernelPML1, highestKernelPML1);

	for (uint64 i = 0; i < lowestKernelPML1; i++) { // Remove unnecessary pages before the kernel
		*indicesToVirtRecursive(511, 510, 0, i) = (paging_entry_t){0};
	}
	for (uint64 i = highestKernelPML1 + 1; i < 512; i++) { // Remove unnecessary pages after the kernel
		*indicesToVirtRecursive(511, 510, 0, i) = (paging_entry_t){0};
	}

	reloadCR3();

	if (upperMapLocation % PAGE_SIZE != 0) {
		upperMapLocation += PAGE_SIZE - (upperMapLocation % PAGE_SIZE);
	}
}
