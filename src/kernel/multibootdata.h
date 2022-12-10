#include <system.h>
#include <multiboot1header.h>

#pragma once

typedef struct displayData_t {
    bool isGraphicalFramebuffer; // If true, the framebuffer is graphical. If false, it's a text console
    void* framebufferVirtAddress; // The virtual address that the framebuffer is mapped to. Use this to access the framebuffer
    uint32 framebufferPhysAddress; // Don't access this address directly, use framebufferVirtAddress instead
    uint64 framebufferSize; // The size of the framebuffer, in bytes
    uint32 width; // The width of the framebuffer, in pixels if graphical, in characters if text
    uint32 height; // The height of the framebuffer, in pixels if graphical, in characters if text
    uint32 depth; // The number of bytes per pixel
    uint32 pitch; // The number of bytes between one pixel and the one directly below it
} displayData_t;

typedef struct bootData_t {
    uint32 multibootMagic; // The magic number that the bootloader passed to the kernel
    uint32 bootDevice;
    char* cmdline; // Any arguments passed to the kernel
    char* bootloaderName; // The name of the bootloader
    void* mmapAddress; // The virtual address of the memory map
    uint32 mmapLength; // The length of the memory map, in bytes
} bootData_t;

bootData_t *getBootData();
displayData_t *getDisplayData();


void parseMultibootData(uint32 bootloaderMagic, uint32 multibootLocation);