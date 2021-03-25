#include <system.h>

#pragma once

void reverse(char s[]);

int itoa(int64 value, char *sp, int radix);

bool isDigit(kchar c);

bool isPrint(kchar c);

uint8 bcdToDecimal(uint8 bcd);

void halt();

void reboot();

#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]

void _assert(kchar *file, uint32 line, kchar *msg, bool conf);
#define assert(conf, msg) _assert(__FILE__, __LINE__, msg, conf)
#define assertf(msg) _assert(__FILE__, __LINE__, msg, false)

void _panic(kchar *file, uint32 line, kchar *msg);
#define panic(msg) _panic(__FILE__, __LINE__, msg)