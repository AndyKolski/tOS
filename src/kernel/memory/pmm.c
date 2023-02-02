#include <formatting.h>
#include <memory/paging.h>
#include <memory/pmm.h>
#include <multibootdata.h>
#include <stdio.h>
#include <string.h>
#include <system.h>

typedef struct mem_ll_entry mem_ll_entry;

struct mem_ll_entry {
	mem_ll_entry *next;
	mem_ll_entry *prev;

	void *regionStart;
	uint64 regionLength;
	bool inUse;
};

C_ASSERT(sizeof(mem_ll_entry) == 40);

mem_ll_entry usableMemory_first = {0};


mem_ll_entry llEntryBuffer[128] = {0};
// TODO: Important! Make this dynamic
// For now the buffer is 128*40 or 5120 bytes, which should be enough for now


// Bitmap of free buffer entries
uint64 buffer_bitmap[2] = {0};

bool buffer_bitmap_get(uint64 index) {
	return (buffer_bitmap[index / 64] & (1 << (index % 64))) != 0;
}
void buffer_bitmap_set(uint64 index, bool value) {
	if (value) {
		buffer_bitmap[index / 64] |= (uint64) (1 << (index % 64));
	} else {
		buffer_bitmap[index / 64] &= (uint64) ~(1 << (index % 64));
	}
}


mem_ll_entry *getNewEntry() {

	for (uint64 i = 0; i < 128; i++) {
		if (!buffer_bitmap_get(i)) {
			buffer_bitmap_set(i, true);
			return &llEntryBuffer[i];
		}
	}
	
	assertf("Ran out of available linked list entries");
	return NULL;
}

void freeEntry(mem_ll_entry *entry) {
	for (uint64 i = 0; i < 128; i++) {
		if (&llEntryBuffer[i] == entry) {
			buffer_bitmap_set(i, false);
			return;
		}
	}
	
	assertf("Tried to free a linked list entry that wasn't allocated");
}

void insertEntry(void *regionStart, uint64 regionLength, bool inUse) { // Insert a new entry into the linked list

	// printf("Inserting usable memory region at 0x%p, length %lu %s\n", regionStart, numBytesToHuman(regionLength), numBytesToUnit(regionLength));
	mem_ll_entry *thisEntry = &usableMemory_first;

	if (thisEntry->regionLength == 0) {
		usableMemory_first.regionStart = regionStart;
		usableMemory_first.regionLength = regionLength;
		usableMemory_first.inUse = inUse;
		return;
	}

	while (thisEntry->next != NULL) {
		if (thisEntry->regionStart + thisEntry->regionLength <= regionStart && thisEntry->next->regionStart >= regionStart) { // If the new region is between this entry and the next entry, insert it here
			mem_ll_entry *newEntry = getNewEntry();
			newEntry->regionStart = regionStart;
			newEntry->regionLength = regionLength;
			newEntry->inUse = inUse;
			newEntry->next = thisEntry->next;
			newEntry->prev = thisEntry;
			if (thisEntry->next != NULL) {
				thisEntry->next->prev = newEntry;
			}
			thisEntry->next = newEntry;
			return;
		}
		thisEntry = thisEntry->next;
	}

	// If we get here, the new region is not between any entries, so we will add it to the end of the list
	mem_ll_entry *newEntry = getNewEntry();
	newEntry->regionStart = regionStart;
	newEntry->regionLength = regionLength;
	newEntry->inUse = inUse;
	newEntry->next = NULL;
	newEntry->prev = thisEntry;
	thisEntry->next = newEntry;
}

/// @brief Prints the memory management linked list. Used for debugging.
void printLL() {
	mem_ll_entry *thisEntry = &usableMemory_first;
	while (thisEntry != NULL) {
		printf("Entry at 0x%p, length %lu %s, inUse %s (Ends at 0x%p)\n", thisEntry->regionStart, numBytesToHuman(thisEntry->regionLength), numBytesToUnit(thisEntry->regionLength), thisEntry->inUse ? "true" : "false", thisEntry->regionStart + thisEntry->regionLength);
		thisEntry = thisEntry->next;
	}
}


/// @brief Merges adjacent free regions.
void mergeFreeRegions() {
	uint64 freedEntries = 0;
	mem_ll_entry *thisEntry = &usableMemory_first;

	while (thisEntry->next != NULL) {
		mem_ll_entry *nextEntry = thisEntry->next;
		if (thisEntry->regionStart + thisEntry->regionLength == nextEntry->regionStart && !thisEntry->inUse && !nextEntry->inUse) { // If the next entry is adjacent to this one, and both are free, merge them
			thisEntry->regionLength += nextEntry->regionLength;
			thisEntry->next = nextEntry->next;
			if (nextEntry->next != NULL) {
				nextEntry->next->prev = thisEntry;
			}
			freeEntry(nextEntry);
			freedEntries++;
		} else {
			thisEntry = nextEntry;
		}
	}
	printf("mergeFreeRegions: Freed %lu entries.\n", freedEntries);
}

/// @brief Gets a pointer to a free 4KB page of memory.
/// @return A pointer to a free 4KB page of memory.
void * getFreePhysicalPage() {
	mem_ll_entry *thisEntry = &usableMemory_first;

	while (thisEntry != NULL) {
		if (thisEntry->regionLength >= PAGE_SIZE && !thisEntry->inUse) {
			if ((uintptr_t)(thisEntry->regionStart) % PAGE_SIZE != 0) {
				// Found a free region, but it is not 4KB aligned. We check if we can split it up
				uintptr_t distanceToAligned = PAGE_SIZE - ((uintptr_t)(thisEntry->regionStart) % PAGE_SIZE);

				if (thisEntry->regionLength > distanceToAligned + PAGE_SIZE) { // If the region is large enough to split
					// Split the region into three regions, the space before the aligned region, the aligned region, and the remaining space after the aligned region
					mem_ll_entry *newEntry_alignedRegion = getNewEntry();
					mem_ll_entry *newEntry_remainingSpaceRegion = getNewEntry();

					newEntry_alignedRegion->regionStart = thisEntry->regionStart + distanceToAligned;
					newEntry_alignedRegion->regionLength = PAGE_SIZE;
					newEntry_alignedRegion->inUse = true;
					newEntry_alignedRegion->next = newEntry_remainingSpaceRegion;
					newEntry_alignedRegion->prev = thisEntry;

					newEntry_remainingSpaceRegion->regionStart = thisEntry->regionStart + distanceToAligned + PAGE_SIZE;
					newEntry_remainingSpaceRegion->regionLength = thisEntry->regionLength - distanceToAligned - PAGE_SIZE;
					newEntry_remainingSpaceRegion->inUse = false;
					newEntry_remainingSpaceRegion->next = thisEntry->next;
					newEntry_remainingSpaceRegion->prev = newEntry_alignedRegion;

					thisEntry->regionLength = distanceToAligned;
					thisEntry->next = newEntry_alignedRegion;


					return newEntry_alignedRegion->regionStart;

				} else { // The region is not large enough to be split. We keep looking for a suitable region
				}
			} else { // The region is already 4KB aligned
				if (thisEntry->regionLength > PAGE_SIZE) { // If the region is large enough to need to be split
					// Split the region into two regions, the aligned region, and the remaining space after the aligned region
					mem_ll_entry *newEntry_remainingSpaceRegion = getNewEntry();

					newEntry_remainingSpaceRegion->regionStart = thisEntry->regionStart + PAGE_SIZE;
					newEntry_remainingSpaceRegion->regionLength = thisEntry->regionLength - PAGE_SIZE;
					newEntry_remainingSpaceRegion->inUse = false;
					newEntry_remainingSpaceRegion->next = thisEntry->next;
					newEntry_remainingSpaceRegion->prev = thisEntry;

					thisEntry->regionLength = PAGE_SIZE;
					thisEntry->next = newEntry_remainingSpaceRegion;
					thisEntry->inUse = true;

					return thisEntry->regionStart;

				} else { // The region is already the correct size, so we can just use it					
					thisEntry->inUse = true;

					return thisEntry->regionStart;
				}
			}
		} else { // The region is smaller than a page, or it is in use

		}

		thisEntry = thisEntry->next;
	}
	assertf("Unable to find a free page of memory");
	return NULL;
}

/// @brief Frees a region of memory. This function will panic if the memory is not in use.
/// @param pointer A pointer to the start of the region of memory to free.
void freePhysicalPage(void *pointer) {
	mem_ll_entry *thisEntry = &usableMemory_first;

	while (thisEntry != NULL) {
		if (thisEntry->regionStart == pointer) { // If this is the entry we are looking for
			if (thisEntry->inUse) {
				thisEntry->inUse = false;
				return;
			} else {
				assertf("Tried to free a region of memory that was not in use");
				return;
			}
		}

		thisEntry = thisEntry->next;
	}
	assertf("Tried to free a region of memory that was not in the list");		
}

/// @brief Gets the amount of free memory in bytes. Optionally, it can exclude regions of memory that are smaller than a page.
/// @param excludeSmallRegions If true, regions of memory smaller than a page will be excluded from the total.
/// @return The amount of free memory in bytes.
uint64 getFreeMemory(bool excludeSmallRegions) {
	uint64 freeBytes = 0;
	mem_ll_entry *thisEntry = &usableMemory_first;

	while (thisEntry != NULL) {
		if (!thisEntry->inUse) {
			if (excludeSmallRegions) {
				if (thisEntry->regionLength >= PAGE_SIZE) {
					freeBytes += thisEntry->regionLength;
				}
			} else {
				freeBytes += thisEntry->regionLength;
			}
		}

		thisEntry = thisEntry->next;
	}
	return freeBytes;
}

void initPMM() {
	bootData_t *bootData = getBootData();
	multiboot_memory_map_t *memoryMap = bootData->mmapAddress;
	
	printf("Found %li memory regions\n", bootData->mmapLength/sizeof(multiboot_memory_map_t));

	for (size_t i = 0; i < bootData->mmapLength/sizeof(multiboot_memory_map_t); i++) {
		if (memoryMap[i].type == MULTIBOOT_MEMORY_AVAILABLE) {
			if (memoryMap[i].addr < MiB) {
				// Ignore the first MiB of memory, there tend to be important structures there
				printf("Ignoring region at 0x%p, length %lu %s\n", (void*)memoryMap[i].addr, numBytesToHuman(memoryMap[i].len), numBytesToUnit(memoryMap[i].len));
			} else {
				// Usable memory entry!

				if ((void*)memoryMap[i].addr < KERNEL_END && (void*)(memoryMap[i].addr + memoryMap[i].len) > KERNEL_START) { // This region contains the kernel, so we split it into two regions
					
					if ((void*)memoryMap[i].addr < KERNEL_START) {
						// The region starts before the kernel, so we add the first part to the linked list
						insertEntry((void*)memoryMap[i].addr, (uintptr_t)KERNEL_START - memoryMap[i].addr, false);
					}
					if ((void*)(memoryMap[i].addr + memoryMap[i].len) > KERNEL_END) {
						// The region ends after the kernel, so we add the second part to the linked list
						insertEntry(KERNEL_END, memoryMap[i].addr + memoryMap[i].len - (uintptr_t)KERNEL_END, false);
					}
				} else { // This region does not contain the kernel, so we can add it to the linked list
					insertEntry((void*)(memoryMap[i].addr), memoryMap[i].len, false);
				}
			}
		} else {
			// Reserved memory - We ignore it for now
		}

	}

	uint64 freeMemory = getFreeMemory(false);

	printf("Kernel start: 0x%08lx end: 0x%08lx len: 0x%lx (%lu %s)\n", (intptr_t)KERNEL_START, (intptr_t)KERNEL_END, KERNEL_SIZE, numBytesToHuman(KERNEL_SIZE), numBytesToUnit(KERNEL_SIZE));
	printf("Total usable free memory: %lu %s\n", numBytesToHuman(freeMemory), numBytesToUnit(freeMemory));
	return;
}