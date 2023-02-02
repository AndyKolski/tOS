#include <display.h>
#include <stddef.h>
#include <string.h>

// inline void *memcpy(void *dest, const void *src, size_t n) {
//     char *dp = dest;
//     const char *sp = src;
//     while (n--)
//         *dp++ = *sp++;
//     return dest;
// }

/// @brief Copies n bytes from src to dest
/// @param dest The destination address, to which the data is copied
/// @param src The source address, from which the data is copied
/// @param n The number of bytes to copy
/// @return A pointer to the destination address
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

/// @brief Sets a region of memory to a given value
/// @param s A pointer to the start of the region to be set
/// @param c The value to set the region to
/// @param n The length of the region to be set
/// @return A pointer to the start of the region
void *memset(void *s, int c, size_t n) {
	unsigned char* p=s;
	while(n--)
		*p++ = (unsigned char)c;
	return s;
}

/// @brief Concatenates two strings together
/// @param destination The string to be appended to
/// @param source The string to be appended
/// @return A pointer to the destination string
char* strcat(char* destination, const char* source) {
	strcpy(destination + strlen(destination), source);
	return destination;
}

/// @brief Compares two strings
/// @param s1 The first string to compare
/// @param s2 The second string to compare
/// @return 0 if the strings are equal, otherwise the difference between the first differing characters
int strcmp(const char* s1, const char* s2) {
	while (*s1 && (*s1 == *s2))
		s1++, s2++;
	return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

/// @brief Compares two strings up to a given length n
/// @param s1 The first string to compare
/// @param s2 The second string to compare
/// @param n The maximum number of characters to compare
/// @return 0 if the strings are equal, otherwise the difference between the first differing characters
int strncmp(const char* s1, const char* s2, size_t n) {
	while(n--)
		if(*s1++!=*s2++)
			return *(unsigned char*)(s1 - 1) - *(unsigned char*)(s2 - 1);
	return 0;
}

/// @brief Copies a string
/// @param dest The destination address, to which the string is copied
/// @param src The source address, from which the string is copied
/// @return A pointer to the destination address
char *strcpy(char *dest, const char* src) {
	char *ret = dest;
	while ((*dest++ = *src++)) {}
	return ret;
}

/// @brief Gets the length of a string
/// @param s The string to get the length of
/// @return The length of the string in bytes or characters
size_t strlen(const char *s) {
	const char *p = s;
	while (*s) ++s;
	return (size_t) (s - p);
}