#include <system.h>

#pragma once

#define FLAG_PAGE_PRESENT (1UL<<0)
#define FLAG_PAGE_WRITABLE (1UL<<1)
#define FLAG_PAGE_USER (1UL<<2)
#define FLAG_PAGE_EXECUTABLE (1UL<<3)
// #define FLAG_PAGE_WRITETHROUGH_CACHE 1<<3
// #define FLAG_PAGE_NOCACHE 1<<4

#define FLAG_PAGE_KERNEL (1UL<<63)

#define PAGE_SIZE (4 * KiB)

void initPaging();

void mapRegion(void* physicalAddress, void* virtualAddress, size_t length, uint64 flags);

void* mapPhysicalToKernel(void* physicalAddress, size_t length, uint64 flags);