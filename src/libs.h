#include <system.h>
#ifndef LIBS_H
# define LIBS_H

uint8 *memcpy(uint8 *dest, const uint8 *src, uint32 count);

void memcpyint(void *dest, void *src, int n) ;

char *memset(char *dest, char val, int count);

short *memsetw(short *dest, short val, int count);

int strlen(const kchar *str);

char* strcpy(kchar* destination, const kchar* source);

void reverse(char s[]);

int itoa(int64 value, char *sp, int radix);

void halt();

void reboot();

#endif