#include <multiboot.h>
#include <system.h>
#pragma once

void install_memory(multiboot_memory_map_t* mmap_addr, uint32 mmap_length, uint32 *kmain);
void *kmalloc(size_t size);
void *kmallocAlligned(size_t size, uint32 alignment);