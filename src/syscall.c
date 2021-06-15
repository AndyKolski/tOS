#include <idt.h>
#include <stdio.h>
#include <syscall.h>
#include <system.h>
#include <time.h>

void install_syscall() {
	extern void user_syscall();

    idt_set_gate(0x80, (unsigned)user_syscall, 0x08, 0xE, false, 3);
}

uint32 syscall(uint32 ebx, uint32 eax) {
    switch (eax) {
        case 1:
            printf("%s", (char*)ebx);
            return 1;
            break;
        case 2:
            printf("%lu", ebx);
            return 1;
            break;
        case 3:
            return ebx+1;
        case 4:
            return (uint32) getUptimeMs();
        default:
            return 0xffffffff;
     }
}