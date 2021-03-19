#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#pragma once

typedef uint8_t			uint8;
typedef uint16_t		uint16;
typedef uint32_t		uint32;
typedef uint64_t		uint64;
typedef int8_t			int8;
typedef int16_t			int16;
typedef int32_t			int32;
typedef int64_t			int64;

typedef char kchar;

/* This defines what the stack looks like after an ISR was running */
struct regs {
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};