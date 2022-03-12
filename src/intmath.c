#include <system.h>

uint32 intdivceil(uint32 a, uint32 b) { // calculates ceil(a/b) without using any floating point math
	if (a%b == 0) {
		return a/b;
	} else {
		return a/b + 1;
	}
}