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

 // TODO: At some point it would be good to transition to using uint32 for 
 // chars, to allow for easier Unicode support in the future.

typedef char kchar;

#define KERNEL_OFFSET 0xC0000000

#define XOR(A, B) (((A) || (B)) && !((A) && (B)))

#define KiB (1024)
#define MiB (KiB * 1024)
#define GiB (MiB * 1024)

/* This defines what the stack looks like after an ISR was running */
struct regs {
	unsigned int gs, fs, es, ds;      /* pushed the segs last */
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
	unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
	unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};


void halt();

void reboot();

#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]

void _assert(kchar *file, uint32 line, const kchar *func, kchar *msg, bool conf);
void  _panic(kchar *file, uint32 line, const kchar *func, kchar *msg);

#define assert(conf, msg) _assert(__FILE__, __LINE__, __func__, msg, conf)
#define assertf(msg)      _assert(__FILE__, __LINE__, __func__, msg, false)

#define panic(msg)         _panic(__FILE__, __LINE__, __func__, msg)