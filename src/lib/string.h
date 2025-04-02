#include <stddef.h>

#pragma once

void *memcpy(void *dest, const void *src, size_t n);

void *memset(void *s, int c, size_t n);

char* strcat(char* destination, const char* source);

char* strncat(char* destination, const char* source, size_t n);

int strcmp(const char* s1, const char* s2);

int strncmp(const char* s1, const char* s2, size_t n);

char *strcpy(char *restrict dest, const char *restrict src);

char *strncpy(char *restrict dest, const char *restrict src, size_t n);

size_t strlen(const char *s);
