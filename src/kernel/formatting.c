#include <system.h>
#include <formatting.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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

void repeatString(char *string, uint32 nTimes) {
	for (uint32 i = 0; i < nTimes; i++) {
		printf("%s", string);
	}
}

uint32 getNumberLength(uint32 num, uint32 base) {
	if (num == 0) {
		return 1;
	}
 
	uint32 digits = 0;
	while (num != 0) {
		num /= base;
		digits++;
	}

	return digits;
}

void hexDump(void *data, uint32 nBytes) {
	#define bytesWide 16
	char hexPrintBuffer[128] = {0};
	char ASCIIPrintBuffer[128] = {0};

	uint32 maxNumWidth = getNumberLength(nBytes, 16);

	repeatString(" ", maxNumWidth + 3);
	for (uint32 i = 0; i < bytesWide; i++) {
		if (i % 8 == 0 && i != 0) {
			printf(" ");
		}
		printf("%02x ", i);
	}
	printf("\n");

	repeatString(" ", maxNumWidth + 3);
	repeatString("-", (bytesWide * 3) + (bytesWide/8) - 2); // Print a line of dashes to visually separate the data below from the 

	printf("\n");


	for (uint32 i = 0; i < nBytes; i+=bytesWide) {
		memset(hexPrintBuffer, 0, sizeof(hexPrintBuffer));
		memset(ASCIIPrintBuffer, 0, sizeof(ASCIIPrintBuffer));


		for (uint32 j = 0; j < bytesWide; j++) {
			uint8 thisByte;
			if (j % 8 == 0 && j != 0) {
				strcat(hexPrintBuffer, " ");
			}

			if (i+j >= nBytes) { // print blank spaces for past the end of the buffer
				strcat(hexPrintBuffer, "   ");

				continue;
			} else {
				thisByte = *(uint8 *)(data+i+j);
			}

			char tempTextBuffer[10];

			sprintf(tempTextBuffer, "%02x ", thisByte);
			strcat(hexPrintBuffer, tempTextBuffer);

			if (isPrint((char)thisByte)) {
				sprintf(tempTextBuffer, "%c", thisByte);
				strcat(ASCIIPrintBuffer, tempTextBuffer);
			} else {
				strcat(ASCIIPrintBuffer, ".");
			}

		}

		printf("%0*x : %s [%s]\n", maxNumWidth, i, hexPrintBuffer, ASCIIPrintBuffer);
	}

	printf("(%d bytes)\n", nBytes);
	return;
}