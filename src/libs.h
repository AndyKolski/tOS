#include <system.h>

#pragma once

void reverse(char s[]);

int itoa(int64 value, char *sp, int radix);

int isDigit (char c);

uint8 bcdToDecimal(uint8 bcd);

void halt();

void reboot();

void _assert(kchar *file, uint32 line, kchar *msg, bool conf);
#define assert(conf, msg) _assert(__FILE__, __LINE__, msg, conf)