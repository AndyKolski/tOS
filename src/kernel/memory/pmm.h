#include <system.h>
#pragma once

extern void* __KERNEL_START;
extern void* __KERNEL_END;

#define KERNEL_START ((void*)&__KERNEL_START)
#define KERNEL_END ((void*)&__KERNEL_END)
#define KERNEL_SIZE (uint64)(KERNEL_END - KERNEL_START)

extern void* __TEXT_START;
extern void* __TEXT_END;

#define TEXT_START ((void*)&__TEXT_START)
#define TEXT_END ((void*)&__TEXT_END)
#define TEXT_SIZE (uint64)(TEXT_END - TEXT_START)

extern void* __RODATA_START;
extern void* __RODATA_END;

#define RODATA_START ((void*)&__RODATA_START)
#define RODATA_END ((void*)&__RODATA_END)
#define RODATA_SIZE (uint64)(RODATA_END - RODATA_START)

extern void* __DATA_START;
extern void* __DATA_END;

#define DATA_START ((void*)&__DATA_START)
#define DATA_END ((void*)&__DATA_END)
#define DATA_SIZE (uint64)(DATA_END - DATA_START)

extern void* __BSS_START;
extern void* __BSS_END;

#define BSS_START ((void*)&__BSS_START)
#define BSS_END ((void*)&__BSS_END)
#define BSS_SIZE (uint64)(BSS_END - BSS_START)

void initPMM();