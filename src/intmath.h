#include <system.h>

#pragma once

uint32 intdivceil(uint32 a, uint32 b);

#define intdivround(a, b) ((a + (b / 2)) / b)
// calculates round(a/b) without using any floating point math
