#include <system.h>

#pragma once

void initISRs();

// Contents of the stack after an interrupt
struct regs {
	uint64 rax, rbx, rcx, rdx, rsi, rdi, rbp, r8, r9, r10, r11, r12, r13, r14, r15;

	uint64 int_no, err_code;

	uint64 rip, cs, rflags, rsp, ss;
};

#define E_DIVISION_BY_ZERO            0
#define E_DEBUG                       1
#define E_NON_MASKABLE_INTERRUPT      2
#define E_BREAKPOINT                  3
#define E_INTO_DETECTED_OVERFLOW      4
#define E_OUT_OF_BOUNDS               5
#define E_INVALID_OPCODE              6
#define E_NO_COPROCESSOR              7
#define E_DOUBLE_FAULT                8
#define E_COPROCESSOR_SEGMENT_OVERRUN 9
#define E_BAD_TSS                     10
#define E_SEGMENT_NOT_PRESENT         11
#define E_STACK_FAULT                 12
#define E_GENERAL_PROTECTION_FAULT    13
#define E_PAGE_FAULT                  14
#define E_UNKNOWN_INTERRUPT           15
#define E_COPROCESSOR_FAULT           16
#define E_ALIGNMENT_CHECK             17
#define E_MACHINE_CHECK               18
