#include <memory/memregion.h>
#include <system.h>

#pragma once

typedef struct displayData_t {
	bool isGraphicalFramebuffer;       // If true, the framebuffer is graphical. If false, it's a text console
	memregion_t framebufferVirtRegion; // The virtual memory region that the framebuffer is mapped to. Use this to access the framebuffer
	memregion_t framebufferPhysRegion; // The physical address that the framebuffer is mapped to. Don't access this address directly, use framebufferVirtAddress instead
	bool isFramebufferMapped;          // If true, the framebuffer is mapped to a virtual address. If false, it's not mapped
	uint32 width;                      // The width of the framebuffer, in pixels if graphical, in characters if text
	uint32 height;                     // The height of the framebuffer, in pixels if graphical, in characters if text
	uint32 depth;                      // The number of bytes per pixel
	uint32 pitch;                      // The number of bytes between one pixel and the one directly below it
} displayData_t;

typedef struct bootData_t {
	char *cmdline;        // Any arguments passed to the kernel
	char *bootloaderName; // The name of the bootloader
} bootData_t;

typedef struct memoryMapEntry_t {
	void *baseAddress;
	uint64 length;
	uint32 type;
} memoryMapEntry_t;

#define MEMORY_AVAILABLE        1
#define MEMORY_RESERVED         2
#define MEMORY_ACPI_RECLAIMABLE 3
#define MEMORY_NVS              4
#define MEMORY_BADRAM           5

static const __attribute__((unused)) char *memoryTypeStringsArray[] = {"", "Available", "Reserved", "ACPI Reclaimable", "ACPI non-volatile storage", "Bad RAM"};

typedef struct memoryMap_t {
	uint32 entryCount;
	uint32 entrySize;
	void *entries;
} memoryMap_t;

bootData_t *getBootData();
displayData_t *getDisplayData();
memoryMap_t *getMemoryMap();

void parseMultibootData(uint32 bootloaderMagic, uint32 multibootPhysLocation);
