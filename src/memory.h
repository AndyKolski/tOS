#include <multiboot.h>
#include <system.h>
#pragma once

void install_memory(multiboot_memory_map_t* mmap_addr, uint32 mmap_length, uint32 *kmain);
