#include <io.h>
#include <libs.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <system.h>

// void reverse(char s[]) {
// 	 uint32 i, j;
// 	 char c;
 
// 	 for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
// 		 c = s[i];
// 		 s[i] = s[j];
// 		 s[j] = c;
// 	 }
//  }

// int itoa(int value, char *sp, int radix) {
//     char tmp[32];// be careful with the length of the buffer
//     char *tp = tmp;
//     int i;
//     unsigned v;

//     int sign = (radix == 10 && value < 0);    
//     if (sign)
//         v = -value;
//     else
//         v = (unsigned)value;

//     while (v || tp == tmp)
//     {
//         i = v % radix;
//         v /= radix; // v/=radix uses less CPU clocks than v=v/radix does
//         if (i < 10)
//           *tp++ = i+'0';
//         else
//           *tp++ = i + 'a' - 10;
//     }

//     int len = tp - tmp;

//     if (sign) 
//     {
//         *sp++ = '-';
//         len++;
//     }

//     while (tp > tmp)
//         *sp++ = *--tp;

//     return len;
// }

uint8 bcdToDecimal(uint8 bcd) {
    assert(((bcd & 0xF0) >> 4) < 10, "invalid BCD conversion");  // More significant nibble is valid
    assert((bcd & 0x0F) < 10, "invalid BCD conversion");         // Less significant nibble is valid
    return ((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F);
}    

void halt() {
	__asm__ volatile ("cli");
	while (true) {
		__asm__ volatile ("hlt");
	}
}

void reboot() {
	 uint8_t good = 0x02;
	 while (good & 0x02)
		  good = inb(0x64);
	 outb(0x64, 0xFE);
	 halt();
}

void _assert(kchar *file, uint32 line, kchar *msg, bool conf) {
	if (!conf) {
		printf("\n [!!!] Assertion failed at %s:%lu - %s\n", file, line, msg);
		halt();
	}
}

void _panic(kchar *file, uint32 line, kchar *msg) {
	printf("\n [!!!] Kernel panic at %s:%lu - %s\n", file, line, msg);
	halt();
}

void panicNoLineNumber(kchar *msg) {
	printf("\n [!!!] Kernel panic - %s\n", msg);
	halt();
}

uintptr_t __stack_chk_guard = 0x681a7261; // Random number, picked using random.org
void __stack_chk_fail(void) {
	panicNoLineNumber("Stack smashing detected!");	
}