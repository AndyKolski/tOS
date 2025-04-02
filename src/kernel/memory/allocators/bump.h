#include <system.h>

#pragma once

extern bool bumpPMM;
extern void* bumpAllocator;
extern void* bumpAllocatorBeginning;

void* ba_getPages(uint32 alignment, uint32 numPages);

void* ba_getPage(uint32 alignment);

#define BA_CONSIDER_REGION(start, length) bumpAllocator = MAX(bumpAllocator, (void*)(start + length))
