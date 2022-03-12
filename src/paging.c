#include <paging.h>
#include <stdio.h>
#include <system.h>
#include <intmath.h>

 // TODO: For now we just use 4-MiB pages. At some point we should transition 
 // to 4-KiB pages.

 // TODO: implement functions to check and remove pages

typedef struct PageDirectoryEntry {
	uint32 attributes	: 8;
	uint32 ignored		: 1;
	uint32 availa		: 1;
	uint32 availb		: 1;
	uint32 availc		: 1;
	// uint32 address		: 20;
	uint32 reserved		: 10;
	uint32 address 		: 10;
} PageDirectoryEntry;

volatile PageDirectoryEntry PageDirectoryTable[1024] __attribute__((aligned(4096)));

PageDirectoryEntry createPageDirectoryEntry(uint8 attributes, uint32 available, uint32 address) {
	PageDirectoryEntry new;
	new.attributes = attributes;
	new.ignored = 0;
	new.availa = available & 1;
	new.availb = available >> 1 & 1;
	new.availc = available >> 2 & 1;
	new.reserved = 0;
	new.address = address;// >> 22;// & 0x3ff;
	// new.address = address >> 12;// & 0x3ff;

	return new;
}

extern void invalidateDirectory();

void mapPage(uint8 attributes, void* physicalAddress, void* virtualAddress, bool doInvalidateDirectory) {

	uint32 tableIndex = (uint32)physicalAddress >> 22;
	uint32 tableAddress = (uint32)virtualAddress >> 22;

	printf("mapping: %08lx p (%lu) -> %08lx v (%lu)\n", (uint32)physicalAddress, tableIndex, (uint32)virtualAddress, tableAddress);
	PageDirectoryTable[tableIndex] = createPageDirectoryEntry(attributes, 0, tableAddress);
	if (doInvalidateDirectory) {
		invalidateDirectory();
	}
}

void mapRegion(uint8 attributes, void* physicalAddress, void* virtualAddress, size_t length) {
	if (!(attributes & FOURMIBPAGE)) {
		panic("Unimplemented: Attempted to map a region without using 4-MiB pages");
	}
	uint32 pageLength = 4*MiB;
	uint32 numberOfPagesNeeded = intdivceil(length, pageLength);

	printf("len: %lu, numPages: %lu\n", length, numberOfPagesNeeded);

	for (uint32 i = 0; i < numberOfPagesNeeded; ++i) {
		mapPage(attributes, physicalAddress + (i*pageLength), virtualAddress + (i*pageLength), false);
	}
	invalidateDirectory();
}
