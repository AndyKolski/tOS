#include <system.h>
#include <stddef.h>

#pragma once

void *memcpy(void *dest, const void *src, size_t n);

void *memset(void *s, int c, size_t n);

char* strcat(char* destination, const char* source);

int strcmp(const char* s1, const char* s2);

int strncmp(const char* s1, const char* s2, size_t n);

char *strcpy(char *restrict dest, const char *restrict src);

size_t strlen(const kchar *s);
