#include <formatting.h>
#include <memory/paging.h>
#include <memory/pmm.h>
#include <multibootdata.h>
#include <stdio.h>
#include <string.h>
#include <system.h>

memoryMap_t* bootloaderMemoryMap = NULL;

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

#define BA_CONSIDER_REGION(start, length) bumpAllocator = MAX(bumpAllocator, (void*)(start + length))

void* getPage() {
	if (bumpPMM) {
		return ba_getPage(PAGE_SIZE);
	} else {
		// We haven't initialised the PMM yet, so we can't allocate memory
		panic("Attempted to allocate memory before the PMM was initialised");
		return NULL;
	}
}

// I'm keeping this around for now...
void* getFreePhysicalPage() {
	return getPage();
}

void initPMM() {
	bootloaderMemoryMap = getMemoryMap();
	uint64 availableRegions = 0;

	printf("Found %u memory regions\n", bootloaderMemoryMap->entryCount);

	// We go over the memory map twice. First to find the last region for the bump allocator, and again to find the
	// usable regions for a yet-to-be-implemented more advanced allocator

	for (uint64 goOver = 1; goOver <= 2; goOver++) {
		for (uint64 entryIndex = 0; entryIndex < bootloaderMemoryMap->entryCount; entryIndex++) {
			memoryMapEntry_t* entry = bootloaderMemoryMap->entries + (entryIndex * bootloaderMemoryMap->entrySize);

			if (goOver == 1) {
				char typeBuffer[32];

				if (entry->type < ARRAY_NUM_ELEMS(memoryTypeStringsArray)) { // Sometimes buggy firmware gives us invalid types. We don't want to crash because of that
					strcpy(typeBuffer, memoryTypeStringsArray[entry->type]);
				} else {
					snprintf(typeBuffer, ARRAY_NUM_ELEMS(typeBuffer), "Unknown type: %u", entry->type);
				}

				DEBUG(printf("Region #%lu: base: 0x%p, size: %4lu %s, type: %s\n", entryIndex + 1, entry->baseAddress, numBytesToHuman(entry->length), numBytesToUnit(entry->length), typeBuffer););
			}

			if (entry->type == MEMORY_AVAILABLE && entry->baseAddress >= (void*)MiB) {
				if (goOver == 1) {
					BA_CONSIDER_REGION(entry->baseAddress, entry->length);
					availableRegions++;
				} else if (goOver == 2) {
					// TODO: Use this region in a more advanced allocator
				}
			}
		}

		if (goOver == 1) {
			bumpAllocatorBeginning = bumpAllocator;
			bumpPMM = true;

			printf("Found %lu available regions\n", availableRegions);
		}
	}
	// printf("Total usable free memory: %lu %s\n", numBytesToHuman(freeMemory),
	// numBytesToUnit(freeMemory));
	return;
}
