#include <paging.h>
#include <stdio.h>
#include <system.h>

 // TODO: For now we just use identity paging and 4-MiB pages. At some point we
 // should transition to a higher-half kernel, with 4-KiB pages.

typedef struct PageDirectoryEntry {
	uint32 present		: 1;
	uint32 rw 			: 1;
	uint32 user			: 1;
	uint32 writeThrough	: 1;
	uint32 cacheDisabled: 1;
	uint32 accessed		: 1;
	uint32 zero			: 1;
	uint32 size 		: 1;
	uint32 ignored		: 1;
	uint32 availa		: 1;
	uint32 availb		: 1;
	uint32 availc		: 1;
	// uint32 address		: 20;
	uint32 reserved		: 10;
	uint32 address 		: 10;
} PageDirectoryEntry;

volatile PageDirectoryEntry PageDirectoryTable[1024] __attribute__((aligned(4096)));

PageDirectoryEntry createPageDirectoryEntry(uint32 present, uint32 rw, uint32 user, uint32 writeThrough, uint32 cacheDisabled, uint32 size, uint32 available, uint32 address) {
	PageDirectoryEntry new;
	new.present = present;
	new.rw = rw;
	new.user = user;
	new.writeThrough = writeThrough;
	new.cacheDisabled = cacheDisabled;
	new.accessed = 0;
	new.zero = 0;
	new.size = size;
	new.ignored = 0;
	new.availa = available & 1;
	new.availb = available >> 1 & 1;
	new.availc = available >> 2 & 1;
	new.reserved = 0;
	new.address = address;// >> 22;// & 0x3ff;
	// new.address = address >> 12;// & 0x3ff;

	return new;
}

extern void enablePaging();

void install_paging() {
		for (uint32 i = 0; i < 1024; ++i) {
		// printf("%04i: %08x - %08x\n", i, i<<22, ((i+1)<<22)-1);
							// present rw user writeThrough cacheDisabled 		size available address
		PageDirectoryTable[i] = createPageDirectoryEntry(1, 1, 1, 1, 1, 	1, 0, i);
	}

	enablePaging();
}