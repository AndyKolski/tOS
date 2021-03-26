#include <system.h>

#pragma once

void puts(kchar *text);
__attribute__ ((format (printf, 1, 2))) int printf (const char* format, ...);
