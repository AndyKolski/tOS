#pragma once

kchar putchar(kchar chr);

void puts(kchar *text);

__attribute__ ((format (printf, 1, 2))) int printf (const char* format, ...);
