#include <system.h>

#pragma once

#define PRESENT (1<<0)
#define WRITABLE (1<<1)
#define USERACCESS (1<<2)
#define WRITETHROUGHCACHE (1<<3)
#define DISABLECACHE (1<<4)
#define ACCESSED (1<<5)
#define DIRTY (1<<6)
#define FOURMIBPAGE (1<<7)


void mapRegion(uint8 attributes, void* physicalAddress, void* virtualAddress, size_t length);