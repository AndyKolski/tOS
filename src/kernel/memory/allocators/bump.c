#include <memory/allocators/bump.h>
#include <memory/paging.h>
#include <stdio.h>
#include <system.h>

bool bumpPMM = false; // If this is true, we can use the bump allocator

// Bump allocator:

void* bumpAllocator = NULL; // The bump allocator will allocate memory in reverse order, starting at the end of usable memory

void* bumpAllocatorBeginning = NULL;
// Points to the first page allocated by the bump allocator.
// We need this so we can calculate how much memory we have allocated so far

void* ba_getPages(uint32 alignment, uint32 numPages) {
	assert(bumpPMM, "Bump allocator not initialised");
	// Reminder: bumpAllocator is the first byte of the last page we allocated.
	// We need to subtract a page size to get the start of the next free page.

	printf("BA allocating %u page(s)\n", numPages);

	bumpAllocator -= (numPages * PAGE_SIZE);
	if ((uintptr_t)bumpAllocator % alignment != 0) {
		bumpAllocator -= (intptr_t)bumpAllocator % alignment;
	}

	return bumpAllocator;
}

void* ba_getPage(uint32 alignment) {
	return ba_getPages(alignment, 1);
}
