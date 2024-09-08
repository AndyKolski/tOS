#include <system.h>

#pragma once

typedef struct memregion_t {
    void *start;
    size_t length;
    bool isVirtual;
} memregion_t;

memregion_t createMemRegion(void *start, size_t length, bool isVirtual);
