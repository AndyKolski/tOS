#include <formatting.h>
#include <memory/allocators/bitmap.h>
#include <memory/allocators/bump.h>
#include <memory/memregion.h>
#include <memory/paging.h>
#include <memory/pmm.h>
#include <multibootdata.h>
#include <stdio.h>
#include <string.h>
#include <system.h>

memoryMap_t* bootloaderMemoryMap = NULL;

void* getPhysicalPage() {
	if (bitmapPMM) {
		return bitmap_getFreePage();
	} else if (bumpPMM) {
		return ba_getPage(PAGE_SIZE);
	} else {
		// We haven't initialised the PMM yet, so we can't allocate memory
		panic("Attempted to allocate memory before the PMM was initialised");
		return NULL;
	}
}

memregion_t getContiguousPhysicalPages(uint32 numPages) {
	if (bitmapPMM) {
		return bitmap_getFreeContiguousPages(numPages);
	} else if (bumpPMM) {
		return createMemRegion(ba_getPages(PAGE_SIZE, numPages), numPages * PAGE_SIZE, false);
	} else {
		// We haven't initialised the PMM yet, so we can't allocate memory
		panic("Attempted to allocate memory before the PMM was initialised");
		return createMemRegion(NULL, 0, false);
	}
}

void initPMM() {
	bootloaderMemoryMap = getMemoryMap();
	uint64 availableRegions = 0;
	uint64 currentAvailableRegion = 0;

	printf("Found %u memory regions\n", bootloaderMemoryMap->entryCount);

	// We go over the memory map twice. First to find the last large region for the bump allocator, and again to find the
	// usable regions for the bitmap allocator
	for (uint64 goOver = 1; goOver <= 2; goOver++) {
		for (uint64 entryIndex = 0; entryIndex < bootloaderMemoryMap->entryCount; entryIndex++) {
			memoryMapEntry_t* entry = bootloaderMemoryMap->entries + (entryIndex * bootloaderMemoryMap->entrySize);

			if (goOver == 1) { // every region, only the first pass
				char typeBuffer[32];

				if (entry->type < ARRAY_NUM_ELEMS(memoryTypeStringsArray)) { // Sometimes buggy firmware gives us invalid types. We don't want to crash because of that
					strcpy(typeBuffer, memoryTypeStringsArray[entry->type]);
				} else {
					snprintf(typeBuffer, ARRAY_NUM_ELEMS(typeBuffer), "Unknown type: %u", entry->type);
				}

				DEBUG(printf("Region #%2lu: base: 0x%p, size: %4lu %3s, type: %s\n", entryIndex + 1, entry->baseAddress, numBytesToHuman(entry->length), numBytesToUnit(entry->length), typeBuffer););
			}

			if (entry->type == MEMORY_AVAILABLE && entry->baseAddress >= (void*)MiB && entry->length > MiB) { // every available region larger than a MiB, starting after the first MiB

				if (goOver == 1) { // only the first pass
					BA_CONSIDER_REGION(entry->baseAddress, entry->length);
					availableRegions++;
				} else if (goOver == 2) { // only the second time
					allocateBitmap(currentAvailableRegion, createMemRegion(entry->baseAddress, entry->length, false));
					currentAvailableRegion++;
				}
			}
		}

		if (goOver == 1) { // at the end of the first pass
			bumpAllocatorBeginning = bumpAllocator;
			bumpPMM = true;

			printf("Found %lu available regions\n", availableRegions);

			// Allocate the bitmap headers
			allocateBitmapHeaders(availableRegions);
		}
	}

	bumpPMM = false;

	// printf("BA: %p, BAB: %p %ld\n", bumpAllocator, bumpAllocatorBeginning, bumpAllocatorBeginning-bumpAllocator);

	bitmap_markRegionAllocated(createMemRegion(bumpAllocator, (size_t)(bumpAllocatorBeginning - bumpAllocator), false));
	bitmap_markRegionAllocated(createMemRegion(KERNEL_START, KERNEL_SIZE, false));

	bitmapPMM = true;

	// printf("Total usable free memory: %lu %s\n", numBytesToHuman(freeMemory),
	// numBytesToUnit(freeMemory));
	return;
}
