#include <system.h>
#include <memory/paging.h>
#include <multibootdata.h>
#include <multiboot1header.h>

multiboot_info_t *multibootData;

bootData_t bootData = {0};
displayData_t displayData = {0};

void parseMultibootData(uint32 bootloaderMagic, uint32 multibootLocation) {

    // Map the multiboot data to a virtual address so we can access it
    multibootData = mapPhysicalToKernel((void*)(uintptr_t)multibootLocation, sizeof(multiboot_info_t), FLAG_PAGE_PRESENT);
    
    bootData.multibootMagic = bootloaderMagic;
    bootData.bootDevice = multibootData->boot_device;
    bootData.mmapLength = multibootData->mmap_length;

    // The multiboot_info_t struct provides us with a pointer to the memory map and strings, so we need to map them to a virtual address

    // They also may not be page-aligned, so we need to calculate the offset from the page boundary

    // Map the memory map
    uint32 mmapAddressPhysical = multibootData->mmap_addr;
    uint32 mmapAddressPhysicalOffset = 0;

	if (mmapAddressPhysical % PAGE_SIZE != 0) {
		mmapAddressPhysicalOffset = (mmapAddressPhysical % PAGE_SIZE);
	}

    bootData.mmapAddress = mapPhysicalToKernel((void*)(uintptr_t)(mmapAddressPhysical - mmapAddressPhysicalOffset), bootData.mmapLength + mmapAddressPhysicalOffset, FLAG_PAGE_PRESENT) + mmapAddressPhysicalOffset;

    // Map the command line. We don't know how long it is, so we'll just map 256 bytes
    uint32 cmdlinePhysical = multibootData->cmdline;
    uint32 cmdlinePhysicalOffset = 0;

    if (cmdlinePhysical % PAGE_SIZE != 0) {
        cmdlinePhysicalOffset = (cmdlinePhysical % PAGE_SIZE);
    }

    bootData.cmdline = mapPhysicalToKernel((void*)(uintptr_t)(cmdlinePhysical - cmdlinePhysicalOffset), 256 + cmdlinePhysicalOffset, FLAG_PAGE_PRESENT) + cmdlinePhysicalOffset;

    // Map the bootloader name. We don't know how long it is, so we'll just map 256 bytes

    uint32 bootloaderNamePhysical = multibootData->boot_loader_name;
    uint32 bootloaderNamePhysicalOffset = 0;

    if (bootloaderNamePhysical % PAGE_SIZE != 0) {
        bootloaderNamePhysicalOffset = (bootloaderNamePhysical % PAGE_SIZE);
    }

    bootData.bootloaderName = mapPhysicalToKernel((void*)(uintptr_t)(bootloaderNamePhysical - bootloaderNamePhysicalOffset), 256 + bootloaderNamePhysicalOffset, FLAG_PAGE_PRESENT) + bootloaderNamePhysicalOffset;

    // Display stuff
    
    displayData.isGraphicalFramebuffer = multibootData->framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_RGB;
    displayData.framebufferPhysAddress = multibootData->framebuffer_addr;
    
    if (displayData.isGraphicalFramebuffer) {
        displayData.width = multibootData->framebuffer_width;
        displayData.height = multibootData->framebuffer_height;
        displayData.depth = multibootData->framebuffer_bpp / 8; // Multiboot gives us the depth in bits, but we want it in bytes
        displayData.pitch = multibootData->framebuffer_pitch;
        displayData.framebufferPhysAddress = multibootData->framebuffer_addr;
        displayData.framebufferSize = displayData.width * displayData.height * displayData.depth;
    } else { // BIOS text mode
        displayData.width = 80;
        displayData.height = 25;
        displayData.depth = 2;
        displayData.pitch = 160;
        displayData.framebufferPhysAddress = 0xB8000;
        displayData.framebufferSize = displayData.pitch * displayData.height;
    }

    displayData.framebufferVirtAddress = mapPhysicalToKernel((void*)(uintptr_t)displayData.framebufferPhysAddress, displayData.framebufferSize, FLAG_PAGE_PRESENT | FLAG_PAGE_WRITABLE);
}

bootData_t *getBootData() {
    return &bootData;
}
displayData_t *getDisplayData() {
    return &displayData;
}