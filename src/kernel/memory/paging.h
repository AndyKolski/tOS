#include <system.h>

#pragma once

#define FLAG_PAGE_PRESENT            (1UL << 0)
#define FLAG_PAGE_WRITABLE           (1UL << 1)
#define FLAG_PAGE_USER               (1UL << 2)
#define FLAG_PAGE_EXECUTABLE         (1UL << 3)
#define FLAG_PAGE_WRITETHROUGH_CACHE (1UL << 4)
#define FLAG_PAGE_CACHE_DISABLE      (1UL << 5)
#define FLAG_PAGE_GLOBAL             (1UL << 6)

#define FLAG_PAGE_KERNEL (1UL << 63)

#define PAGE_SIZE (4 * KiB)

typedef struct paging_entry_t {
	uint64 present  : 1;  // If set, page is available
	uint64 rw       : 1;  // If set, page is writable, otherwise read-only
	uint64 user     : 1;  // If set, page is accessible from user mode
	uint64 pwt      : 1;  // If set, page uses write-through caching, otherwise write-back caching is used
	uint64 pcd      : 1;  // If set, caching is disabled
	uint64 accessed : 1;  // Enabled by the CPU if a page is accessed
	uint64 ignored  : 1;  // Ignored by the CPU, available for use
	uint64 ps       : 1;  // Reserved for PML4 entries

	uint64 global   : 1;   // If set, page is not flushed from the TLB when CR3 is loaded
	uint64 ignored2 : 3;   // Ignored by the CPU, available for use
	uint64 addr     : 40;  // Address of next level page table
	uint64 ignored3 : 11;  // Ignored by the CPU, available for use
	uint64 nx       : 1;   // If set, page is not executable
} paging_entry_t;

void initPaging();

void mapRegion(void* physicalAddress, void* virtualAddress, size_t length, uint64 flags);

void* mapPhysicalToKernel(void* physicalAddress, size_t length, uint64 flags);

void getVirtualAddressInfo(void* virtualAddress);