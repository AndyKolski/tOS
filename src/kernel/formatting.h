#include <system.h>

#pragma once

uint64 numBytesToHuman(uint64 numBytes);
const char *numBytesToUnit(uint64 numBytes);

const char *boolToString(bool b);

void repeatString(const char *string, uint32 nTimes);

uint32 getNumberLength(uint32 num, uint32 base);

void hexDump(void *data, uint32 nBytes);

#define BOOL_TO_STRING(b) (b ? "true" : "false")