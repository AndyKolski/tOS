#include <system.h>
#include <intmath.h>

uint32 intDivCeil(uint32 a, uint32 b) { // calculates ceil(a/b) without using any floating point math
	if (a%b == 0) {
		return a/b;
	} else {
		return a/b + 1;
	}
}


/// @brief Divides a number of bytes into a more human-readable unit
/// @param numBytes The number of bytes to convert
/// @return The number of bytes in a more appropriate unit
uint64 numBytesToHuman(uint64 numBytes) {
	if (numBytes >= GiB) {
		return intDivRound(numBytes, GiB);
	} else if (numBytes >= MiB) {
		return intDivRound(numBytes, MiB);
	} else if (numBytes >= KiB) {
		return intDivRound(numBytes, KiB);
	} else {
		return numBytes;
	}
}

/// @brief Returns the unit of a number of bytes
/// @param numBytes The number of bytes to convert
/// @return The unit of the number of bytes
const char *numBytesToUnit(uint64 numBytes) {
	if (numBytes >= GiB) {
		return "GiB";
	} else if (numBytes >= MiB) {
		return "MiB";
	} else if (numBytes >= KiB) {
		return "KiB";
	} else {
		return "B";
	}
}