#include <display.h>
#include <irq.h>
#include <stddef.h>
#include <string.h>

// inline void *memcpy(void *dest, const void *src, size_t n) {
//     char *dp = dest;
//     const char *sp = src;
//     while (n--)
//         *dp++ = *sp++;
//     return dest;
// }

inline void *memcpy(void *dest, const void *src, size_t n) {
	asm volatile ("rep movsb"
				: "=D" (dest),
				  "=S" (src),
				  "=c" (n)
				: "0" (dest),
				  "1" (src),
				  "2" (n)
				: "memory");
  return dest;
}

void *memset(void *s, int c, size_t n) {
	unsigned char* p=s;
	while(n--)
		*p++ = (unsigned char)c;
	return s;
}

char* strcat(char* destination, const char* source) {
	strcpy(destination + strlen(destination), source);
	return destination;
}

int strcmp(const char* s1, const char* s2) {
	while (*s1 && (*s1 == *s2))
		s1++, s2++;
	return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
	while(n--)
		if(*s1++!=*s2++)
			return *(unsigned char*)(s1 - 1) - *(unsigned char*)(s2 - 1);
	return 0;
}

char *strcpy(char *dest, const char* src) {
	char *ret = dest;
	while ((*dest++ = *src++)) {}
	return ret;
}

size_t strlen(const char *s) {
	const char *p = s;
	while (*s) ++s;
	return (size_t) (s - p);
}