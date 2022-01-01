[BITS 32]

global bootstrap_entry
bootstrap_entry:
	mov esp, stack_top
	push ebx
	push eax
	fninit
	extern kmain
	call kmain
	jmp $


SECTION .bss
ALIGN 16
stack_bottom:
resb 8192
stack_top: