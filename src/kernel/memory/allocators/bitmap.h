#include <system.h>
#include <memory/memregion.h>

extern bool bitmapPMM;

void allocateBitmapHeaders(uint64 numHeaders);

void allocateBitmap(uint64 headerIndex, memregion_t representedRegion);

void* bitmap_getFreePage();

void bitmap_freePages(memregion_t pages);

memregion_t bitmap_getFreeContiguousPages(uint64 numPages);

void bitmap_markRegionAllocated(memregion_t region);

void bitmap_printStats();