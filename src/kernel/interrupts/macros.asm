[BITS 64]

; Macro to push the CPU state to the stack for interrupts
; This is a macro because it is used in both the interrupt handler and the
; exception handler

%macro push_cpu_state 0

    ; The CPU pushed RSP, RFLAGS, CS, and RIP for us. If the interrupt
    ;  was an exception, it also pushed an error code. Otherwise, we 
    ; already pushed 0x00 to fill the space. We also pushed the interupt 
    ; number. Now we just push the rest of the registers, and put a pointer
    ; to the top of the stack into RSP so that we can access the register
    ; values as a struct passed to the interrupt handler

	push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	push rbp
	push rdi
	push rsi
	push rdx
	push rcx
	push rbx
	push rax

	mov rdi, rsp
%endmacro

; Macro to pop the CPU state from the stack for interrupts

%macro pop_cpu_state 0

    ; Pop the registers in reverse order and add 16 to the stack pointer
    ; to account for the error code and interrupt number
    
	pop rax
	pop rbx
	pop rcx
	pop rdx
	pop rsi
	pop rdi
	pop rbp
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15

	add rsp, 0x10
%endmacro