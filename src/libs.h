#include <system.h>
#ifndef LIBS_H
# define LIBS_H

void reverse(char s[]);

int itoa(int64 value, char *sp, int radix);

int isdigit (char c);

void halt();

void reboot();

void _assert(kchar *file, uint32 line, kchar *msg, bool conf);
#define assert(conf, msg) _assert(__FILE__, __LINE__, msg, conf)

#endif