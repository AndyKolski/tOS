[BITS 64]

global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31

isr0: ; Divide By Zero Exception
	push byte 0
	push byte 0
	jmp isr_common_stub
isr1: ; Debug Exception
	push byte 0
	push byte 1
	jmp isr_common_stub
isr2: ; NMI
	push byte 0
	push byte 2
	jmp isr_common_stub
isr3: ; Breakpoint
	push byte 0
	push byte 3
	jmp isr_common_stub
isr4: ; Into Detected Breakpoint
	push byte 0
	push byte 4
	jmp isr_common_stub
isr5: ; Out of Bounds
	push byte 0
	push byte 5
	jmp isr_common_stub
isr6: ; Debug Exception
	push byte 0
	push byte 6
	jmp isr_common_stub
isr7: ; No Coprocessor
	push byte 0
	push byte 7
	jmp isr_common_stub
isr8: ; Double Fault (With Error Code!)
	push byte 8
	jmp isr_common_stub
isr9: ; Coprocessor Segment Overrun
	push byte 0
	push byte 9
	jmp isr_common_stub
isr10: ; Bad TSS
	push byte 10
	jmp isr_common_stub
isr11: ; Segment Not Present
	push byte 11
	jmp isr_common_stub
isr12: ; Stack Fault
	push byte 12
	jmp isr_common_stub
isr13: ; General Protection Fault
	push byte 13
	jmp isr_common_stub
isr14: ; Page Fault
	push byte 14
	jmp isr_common_stub
isr15: ; Unknown Interupt
	push byte 0
	push byte 15
	jmp isr_common_stub
isr16: ; Coprocessor Fault
	push byte 0
	push byte 16
	jmp isr_common_stub
isr17: ; Alignment Check (486+)
	push byte 0
	push byte 17
	jmp isr_common_stub
isr18: ; Machine Check (Pentium / 586+)
	push byte 0
	push byte 18
	jmp isr_common_stub
isr19: ; Reserved
	push byte 0
	push byte 19
	jmp isr_common_stub
isr20: ; Reserved
	push byte 0
	push byte 20
	jmp isr_common_stub
isr21: ; Reserved
	push byte 0
	push byte 21
	jmp isr_common_stub
isr22: ; Reserved
	push byte 0
	push byte 22
	jmp isr_common_stub
isr23: ; Reserved
	push byte 0
	push byte 23
	jmp isr_common_stub
isr24: ; Reserved
	push byte 0
	push byte 24
	jmp isr_common_stub
isr25: ; Reserved
	push byte 0
	push byte 25
	jmp isr_common_stub
isr26: ; Reserved
	push byte 0
	push byte 26
	jmp isr_common_stub
isr27: ; Reserved
	push byte 0
	push byte 27
	jmp isr_common_stub
isr28: ; Reserved
	push byte 0
	push byte 28
	jmp isr_common_stub
isr29: ; Reserved
	push byte 0
	push byte 29
	jmp isr_common_stub
isr30: ; Reserved
	push byte 0
	push byte 30
	jmp isr_common_stub
isr31: ; Reserved
	push byte 0
	push byte 31
	jmp isr_common_stub

extern fault_handler

%include "src/kernel/interrupts/macros.asm"

isr_common_stub:
	cli

	push_cpu_state

	call fault_handler

	pop_cpu_state

	iretq
			