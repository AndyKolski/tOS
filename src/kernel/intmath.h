#include <system.h>

#pragma once

uint32 intDivCeil(uint32 a, uint32 b);

#define intDivRound(a, b) ((a + (b / 2)) / b)
// calculates round(a/b) without using any floating point math


uint64 numBytesToHuman(uint64 numBytes);
const char *numBytesToUnit(uint64 numBytes);