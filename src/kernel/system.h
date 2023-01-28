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

extern void* _gdt_kernel_code_segment;
#define gdt_kernel_code_segment (uint64)&_gdt_kernel_code_segment
extern void* _gdt_kernel_data_segment;
#define gdt_kernel_data_segment (uint64)&_gdt_kernel_data_segment

extern void* __OFFSET;
#define KERNEL_OFFSET (uintptr_t)&__OFFSET

#define XOR(A, B) (((A) || (B)) && !((A) && (B)))

#define KiB (1024)
#define MiB (KiB * 1024)
#define GiB (MiB * 1024)

/* This defines what the stack looks like after an ISR was running */
struct regs {
	uint64 rax, rbx, rcx, rdx, rsi, rdi, rbp, r8, r9, r10, r11, r12, r13, r14, r15;

	uint64 int_no, err_code;

	uint64 rip, cs, rflags, rsp, ss;
};


void sti();
void cli();

void halt();

void reboot();

#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]

void _assert(char *file, uint32 line, const char *func, char *msg, bool conf);
void  _panic(char *file, uint32 line, const char *func, char *msg);

#define assert(conf, msg) _assert(__FILE__, __LINE__, __func__, msg, conf)
#define assertf(msg)      _assert(__FILE__, __LINE__, __func__, msg, false)

#define panic(msg)         _panic(__FILE__, __LINE__, __func__, msg)