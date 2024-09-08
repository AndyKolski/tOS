#include <system.h>

#pragma once

typedef struct memregion_t {
    void *start;
    size_t length;
    bool isVirtual;
} memregion_t;

memregion_t createMemRegion(void *start, size_t length, bool isVirtual);

bool memRegionContainsAddress(memregion_t region, void *ptr);

bool memRegionOverlaps(memregion_t region1, memregion_t region2);

bool memRegionContainsRegion(memregion_t region1, memregion_t region2);

bool memRegionIsAdjacent(memregion_t region1, memregion_t region2);