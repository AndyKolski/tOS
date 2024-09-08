#include <memory/memregion.h>
#include <system.h>

/// @brief Creates a new memory region
/// @param start The address of the first bite of the region
/// @param length The length of the region
/// @param isVirtual Whether the region is virtual or physical memory. Comparison functions will assert if this is not the same for both regions
/// @return The memory region
memregion_t createMemRegion(void *start, size_t length, bool isVirtual) {
    memregion_t region;
    region.start = start;
    region.length = length;
    region.isVirtual = isVirtual;
    return region;
}
