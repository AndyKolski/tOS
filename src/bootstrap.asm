[BITS 32]

global bootstrap_entry
bootstrap_entry:
	mov esp, stack_top
	push ebx
	push eax

	mov eax, cr0 ; Configure and initialize the FPU before jumping to the C kernel
	and eax, ~0b1100
	or eax, 0b110010
	mov cr0, eax
	mov eax, cr4
	or eax, 0b11000000000
	mov cr4, eax
	fninit

	extern kmain
	call kmain
	jmp $


SECTION .bss
ALIGN 16
stack_bottom:
resb 8192
stack_top: