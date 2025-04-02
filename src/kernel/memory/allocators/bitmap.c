#include <formatting.h>
#include <memory/allocators/bitmap.h>
#include <memory/memregion.h>
#include <memory/paging.h>
#include <memory/pmm.h>
#include <stdio.h>
#include <string.h>
#include <system.h>

typedef struct bitmap_header_t {
	void* regionStart;   // Start of the region of physical memory that the bitmap represents
	size_t regionLengthPages; // The length of the region in pages, and the number of bits in the bitmap
	uint8* bitmap;       // The actual bitmap
} bitmap_header_t;

bitmap_header_t* bitmapHeaders = NULL;
uint64 bitmapCount = 0;
bool bitmapPMM = false;

void allocateBitmapHeaders(uint64 numHeaders) {
	printf("Allocating space for %lu bitmap headers\n", numHeaders);

	// Map memory for headers. This probably will use more memory than we need, but it will never waste more than a single 4-KiB page, so it's not a big deal since it only happens once
	memregion_t headerRegion = mapPhysicalToKernel(getContiguousPhysicalPages(intDivCeil(numHeaders * sizeof(bitmap_header_t), PAGE_SIZE)), FLAG_PAGE_PRESENT | FLAG_PAGE_WRITABLE);

	memset(headerRegion.start, 0, headerRegion.length); // Clear the headers

	// Set the global variables
	bitmapHeaders = (bitmap_header_t*)headerRegion.start;
	bitmapCount = numHeaders;
}

void allocateBitmap(uint64 headerIndex, memregion_t representedRegion) {
	assert(bitmapHeaders != NULL, "Bitmap headers have not been allocated yet");
	assert(headerIndex < bitmapCount, "Bitmap header index is out of bounds");
	assert(representedRegion.isVirtual == false, "Cannot allocate a bitmap for a virtual region");
	assert(representedRegion.start != NULL, "Cannot allocate a bitmap for a NULL region");
	assert((uint64)representedRegion.start % PAGE_SIZE == 0, "Region start address is not 4KB aligned");

	printf("Allocating bitmap for region starting at 0x%p\n", representedRegion.start);

	// Calculate the size of the region in pages
	// Note, we explicitly don't use intDivCeil here, because we don't want to assume the memory region is longer than it is
	uint64 regionLengthPages = representedRegion.length / PAGE_SIZE; // The number of pages in the region or bits in the bitmap
	assert(regionLengthPages > 0, "Cannot allocate a bitmap for a region smaller than a page");

	uint64 bitmapLength = intDivCeil(regionLengthPages, 8); // The size of the bitmap in bytes

	// Allocate the bitmap
	memregion_t bitmapRegion = mapPhysicalToKernel(getContiguousPhysicalPages(intDivCeil(bitmapLength, PAGE_SIZE)), FLAG_PAGE_PRESENT | FLAG_PAGE_WRITABLE);

	memset(bitmapRegion.start, 0, bitmapRegion.length); // Clear the bitmap

	bitmapHeaders[headerIndex].regionStart = representedRegion.start;
	bitmapHeaders[headerIndex].regionLengthPages = regionLengthPages;
	bitmapHeaders[headerIndex].bitmap = (uint8*)bitmapRegion.start;
}

bitmap_header_t* getBitmapHeaderForAddress(void* address) {
	assert(bitmapHeaders != NULL, "Bitmap headers have not been allocated yet");

	for (uint64 i = 0; i < bitmapCount; i++) {
		if (memRegionContainsAddress(createMemRegion(bitmapHeaders[i].regionStart, bitmapHeaders[i].regionLengthPages * PAGE_SIZE, false), address)) {
			return &bitmapHeaders[i];
		}
	}
	printf("Address not found in any bitmap\n");

	return NULL;
}

inline bool isPageAllocated(bitmap_header_t* header, uint64 pageIndex) {
	return (header->bitmap[pageIndex / 8] & (1 << (pageIndex % 8))) != 0;
}

inline void setPageAllocated(bitmap_header_t* header, uint64 pageIndex, bool allocated) {
	if (allocated) {
		header->bitmap[pageIndex / 8] |= (1 << (pageIndex % 8)); // Set the bit
	} else {
		header->bitmap[pageIndex / 8] &= ~(1 << (pageIndex % 8)); // Clear the bit
	}
}

inline void setAddressAllocated(void* address, bool allocated) {
	assert(address != NULL, "Cannot allocate or free a NULL address");
	assert(bitmapHeaders != NULL, "Bitmap headers have not been allocated yet");

	bitmap_header_t* header = getBitmapHeaderForAddress(address);
	assert(header != NULL, "Address not found in any bitmap");

	uint64 pageIndex = ((uint64)address - (uint64)header->regionStart) / PAGE_SIZE;
	setPageAllocated(header, pageIndex, allocated);
}


void* bitmap_getFreePage() {
	assert(bitmapHeaders != NULL, "Bitmap headers have not been allocated yet");
	assert(bitmapCount > 0, "No bitmaps have been allocated yet");

	for (uint64 bitmapHeaderIndex = 0; bitmapHeaderIndex < bitmapCount; bitmapHeaderIndex++) {
		bitmap_header_t* thisHeader = &bitmapHeaders[bitmapHeaderIndex];

		// This is a very naive implementation that just looks for the first free page in the bitmap. It's very inefficient, but it's good enough for now
		// TODO: Implement a more efficient algorithm
		for (uint64 testBit = 0; testBit < thisHeader->regionLengthPages; testBit++) {
			if (!isPageAllocated(thisHeader, testBit)) {
				setPageAllocated(thisHeader, testBit, true);
				return (void*)((uint64)thisHeader->regionStart + testBit * PAGE_SIZE);
			}
		}
	}
	panic("Out of memory");
	return NULL;
}

memregion_t bitmap_getFreeContiguousPages(uint64 numPages) {
	assert(bitmapHeaders != NULL, "Bitmap headers have not been allocated yet");
	assert(bitmapCount > 0, "No bitmaps have been allocated yet");
	assert(numPages > 0, "Cannot allocate 0 pages");

	for (uint64 bitmapHeaderIndex = 0; bitmapHeaderIndex < bitmapCount; bitmapHeaderIndex++) {
		bitmap_header_t* thisHeader = &bitmapHeaders[bitmapHeaderIndex];

		uint64 freePagesFound = 0;
		uint64 firstFreePageIndex = 0;

		// This implementation is just as bad as the above one.
		// TODO: Implement a more efficient algorithm

		for (uint64 testBit = 0; testBit < thisHeader->regionLengthPages; testBit++) {
			if (!isPageAllocated(thisHeader, testBit)) {
				if (freePagesFound == 0) {
					firstFreePageIndex = testBit;
				}
				freePagesFound++;
			} else {
				freePagesFound = 0;
			}

			if (freePagesFound == numPages) {
				for (uint64 i = 0; i < numPages; i++) {
					setPageAllocated(thisHeader, firstFreePageIndex + i, true);
				}
				return createMemRegion((void*)((uint64)thisHeader->regionStart + firstFreePageIndex * PAGE_SIZE), numPages * PAGE_SIZE, false);
			}
		}
	}

	panic("Out of memory");
	return createMemRegion(NULL, 0, false);
}

void bitmap_freePage(void* page) {
	assert(page != NULL, "Cannot free a NULL page");
	setAddressAllocated(page, false);
}
void bitmap_freePages(memregion_t pages) {
	assert(pages.start != NULL, "Cannot free a NULL region");
	assert(pages.length % PAGE_SIZE == 0, "Cannot free a region that is not a multiple of 4KB");

	for (uint64 i = 0; i < pages.length / PAGE_SIZE; i++) {
		setAddressAllocated((void*)((uint64)pages.start + i * PAGE_SIZE), false);
	}
}

// used to mark important areas as allocated so that nothing will allocate over them
void bitmap_markRegionAllocated(memregion_t region) {
	assert(region.start != NULL, "Cannot mark a NULL region as allocated");
	assert(region.start != NULL, "Cannot mark a region with a NULL start address");

	for (uint64 i = 0; i < intDivCeil(region.length, PAGE_SIZE); i++) {
		setAddressAllocated((void*)((uint64)region.start + i * PAGE_SIZE), true);
	}
}

void bitmap_printStats() {
	assert(bitmapHeaders != NULL, "Bitmap headers have not been allocated yet");

	uint64 allocatedPages = 0;
	uint64 totalPages = 0;

	for (uint64 bitmapHeaderIndex = 0; bitmapHeaderIndex < bitmapCount; bitmapHeaderIndex++) {
		bitmap_header_t* thisHeader = &bitmapHeaders[bitmapHeaderIndex];

		for (uint64 testBit = 0; testBit < thisHeader->regionLengthPages; testBit++) {
			if (isPageAllocated(thisHeader, testBit)) {
				allocatedPages++;
			}
			totalPages++;
		}
	}
	printf("Allocated pages: %lu - %lu %s\n", allocatedPages, numBytesToHuman(allocatedPages * PAGE_SIZE), numBytesToUnit(allocatedPages * PAGE_SIZE));
	printf("Total pages: %lu - %lu %s\n", totalPages, numBytesToHuman(totalPages * PAGE_SIZE), numBytesToUnit(totalPages * PAGE_SIZE));
}
