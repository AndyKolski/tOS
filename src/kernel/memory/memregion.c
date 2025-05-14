#include <memory/memregion.h>
#include <system.h>

/// @brief Creates a new memory region
/// @param start The address of the first byte of the region
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

/// @brief Checks if a memory region contains a specific address
/// @param region The memory region to check
/// @param ptr The address to check
/// @return  true if the address is within the region, false otherwise
bool memRegionContainsAddress(memregion_t region, void *ptr) {
    return ptr >= region.start && ptr < region.start + region.length;
}

/// @brief Checks if two memory regions overlap
/// @param region1 The first memory region
/// @param region2 The second memory region
/// @return true if the regions overlap, false otherwise
/// @note This function will assert if the regions are not of the same type (virtual or physical)
bool memRegionOverlaps(memregion_t region1, memregion_t region2) {
    assert(region1.isVirtual == region2.isVirtual, "Cannot compare virtual and physical memory regions");
    return region1.start < region2.start + region2.length && region1.start + region1.length > region2.start;
}

/// @brief Checks if a memory region contains another memory region
/// @param region1 The first memory region
/// @param region2 The second memory region
/// @return true if region2 is completely contained within region1, false otherwise
/// @note This function will assert if the regions are not of the same type (virtual or physical)
bool memRegionContainsRegion(memregion_t region1, memregion_t region2) {
    assert(region1.isVirtual == region2.isVirtual, "Cannot compare virtual and physical memory regions");
    return region1.start <= region2.start && region1.start + region1.length >= region2.start + region2.length;
}

/// @brief Checks if two memory regions are adjacent, i.e. if the end of one region is the start of the other, with no gap in between
/// @param region1 The first memory region
/// @param region2 The second memory region
/// @return true if the regions are adjacent, false otherwise
/// @note This function will assert if the regions are not of the same type (virtual or physical)
bool memRegionIsAdjacent(memregion_t region1, memregion_t region2) {
    assert(region1.isVirtual == region2.isVirtual, "Cannot compare virtual and physical memory regions");
    return region1.start + region1.length == region2.start || region2.start + region2.length == region1.start;
}

