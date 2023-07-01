#include <formatting.h>
#include <memory/paging.h>
#include <memory/pmm.h>
#include <multiboot2.h>
#include <multibootdata.h>
#include <stdio.h>
#include <system.h>

void *multibootData;

bootData_t bootData = {0};
displayData_t displayData = {0};
memoryMap_t memMap = {0};

struct multibootInformationStructureData {
	uint32 total_size;
	uint32 reserved;
};

void parseMultibootData(uint32 bootloaderMagic, uint32 multibootPhysLocation) {
	printf("Multiboot data physical location: 0x%x\n", multibootPhysLocation);

	if (bootloaderMagic != MULTIBOOT2_BOOTLOADER_MAGIC) {
		printf("Incorrect bootloader magic: 0x%x. Expected %x\n", bootloaderMagic, MULTIBOOT2_BOOTLOADER_MAGIC);
	}

	// The multiboot data is likely not page-aligned, so we need to calculate the offset from the page boundary
	uint32 offset = 0;
	if (multibootPhysLocation % 0x1000 != 0) {
		offset = multibootPhysLocation % 0x1000;
	}

	// We map the multiboot data right after the kernel, since we know we have some free memory there
	multibootData = KERNEL_OFFSET + KERNEL_END + 0x1000;

	// Map the multiboot data to a virtual address so we can access it. We don't know how large the multiboot data is, so we map 8 KiB and hope that is enough.
	// TODO: Read the length, and map the exact amount of memory needed
	mapRegion((void *)(uintptr_t)multibootPhysLocation - offset, multibootData, (8 * KiB) + offset, FLAG_PAGE_PRESENT);

	multibootData += offset;

	struct multibootInformationStructureData *multibootInformationStructureData = multibootData;

	assert(multibootInformationStructureData->total_size < 8 * KiB, "Multiboot data is too large (> 8 KiB)");

	// hexDump(multibootData, multibootInformationStructureData->total_size);

	struct multiboot_header_tag *tag = multibootData + sizeof(struct multibootInformationStructureData);
	struct multiboot_tag_string *stringTag;
	while (tag->type != MULTIBOOT_TAG_TYPE_END) {
		// printf("Found tag with type: %d, size: %d\n", tag->type, tag->size);
		switch (tag->type) {
			case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
				stringTag = (struct multiboot_tag_string *)tag;
				bootData.bootloaderName = stringTag->string;
				// printf("Bootloader name: %s\n", bootData.bootloaderName);
				break;
			case MULTIBOOT_TAG_TYPE_CMDLINE:
				stringTag = (struct multiboot_tag_string *)tag;
				bootData.cmdline = stringTag->string;
				// printf("Command line: %s\n", bootData.cmdline);
				break;

			case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
				struct multiboot_tag_framebuffer *framebufferTag = (struct multiboot_tag_framebuffer *)tag;
				displayData.isGraphicalFramebuffer = framebufferTag->common.framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_RGB;
				displayData.framebufferPhysAddress = framebufferTag->common.framebuffer_addr;
				displayData.framebufferSize = framebufferTag->common.framebuffer_pitch * framebufferTag->common.framebuffer_height;
				displayData.width = framebufferTag->common.framebuffer_width;
				displayData.height = framebufferTag->common.framebuffer_height;
				displayData.pitch = framebufferTag->common.framebuffer_pitch;
				displayData.depth = framebufferTag->common.framebuffer_bpp / 8;
				// We wait to page in the framebuffer until it's needed and when advanced paging is available
				// printf("Framebuffer: %d x %d, depth: %d, pitch: %d\n", displayData.width, displayData.height, displayData.depth, displayData.pitch);
				break;

			case MULTIBOOT_TAG_TYPE_MMAP:
				struct multiboot_tag_mmap *mmapTag = (struct multiboot_tag_mmap *)tag;

				memMap.entryCount = (mmapTag->size - sizeof(struct multiboot_tag_mmap)) / mmapTag->entry_size;
				memMap.entrySize = mmapTag->entry_size;
				memMap.entries = (memoryMapEntry_t *)mmapTag->entries;
				// printf("Memory map: version %d, %d entries, each entry is %d bytes\n", mmapTag->entry_version, memMap.entryCount, mmapTag->entry_size);
				break;

			default:  // Ignore all other tags
				break;
		}

		tag = (void *)tag + ((tag->size + 7) & ~7ul);
	}
}

bootData_t *getBootData() {
	return &bootData;
}

bool isFramebufferMapped = false;

displayData_t *getDisplayData() {
	printf("Framebuffer address: 0x%x, size: 0x%lx\n", displayData.framebufferPhysAddress, displayData.framebufferSize);

	if (!isFramebufferMapped) {
		displayData.framebufferVirtAddress = mapPhysicalToKernel((void *)(uintptr_t)displayData.framebufferPhysAddress, displayData.framebufferSize, FLAG_PAGE_PRESENT | FLAG_PAGE_WRITABLE | FLAG_PAGE_WRITETHROUGH_CACHE);
		isFramebufferMapped = true;
	}
	return &displayData;
}

memoryMap_t *getMemoryMap() {
	return &memMap;
}
