#include <system.h>

#pragma once

#define FLAG_PAGE_PRESENT (1UL<<0)
#define FLAG_PAGE_WRITABLE (1UL<<1)
#define FLAG_PAGE_USER (1UL<<2)
#define FLAG_PAGE_EXECUTABLE (1UL<<3)
#define FLAG_PAGE_WRITETHROUGH_CACHE (1UL<<4)
#define FLAG_PAGE_CACHE_DISABLE (1UL<<5)
#define FLAG_PAGE_GLOBAL (1UL<<6)

#define FLAG_PAGE_KERNEL (1UL<<63)

#define PAGE_SIZE (4 * KiB)

void initPaging();

void mapRegion(void* physicalAddress, void* virtualAddress, size_t length, uint64 flags);

void* mapPhysicalToKernel(void* physicalAddress, size_t length, uint64 flags);