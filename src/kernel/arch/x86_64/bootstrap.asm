OFFSET equ 0xFFFFFFFF7FF00000 ; Kernel at 0xffffffff80000000

PRESENT equ 1<<0
WRITABLE equ 1<<1
USERACCESS equ 1<<2
WRITETHROUGHCACHE equ 1<<3
DISABLECACHE equ 1<<4
ACCESSED equ 1<<5
DIRTY equ 1<<6
PAGESIZE equ 1<<7
PAT equ 1<<7

; db is 8 bits or 1 byte
; dw is 16 bits or 2 bytes
; dd is 32 bits or 4 bytes
; dq is 64 bits or 8 bytes

SECTION .rodata

; GDT descriptor structure:
; 0:15 - limit (2 bytes)
; 16:31 - base (2 bytes)

; 32:39 - base (1 byte)
; 40:47 - access byte (1 byte)
; 48:55 - granularity byte (1 byte) [4:7 - limit (4 bits), 0:3 - flags (4 bits)]
; 56:63 - base (1 byte)


ALIGN 8

gdt_start:

	; null descriptor
	dq 0
global _gdt_kernel_code_segment
_gdt_kernel_code_segment: equ $ - gdt_start

	; code descriptor
	dw 0xffff
	dw 0x0000

	db 0x00
	db 0b10011010
	db 0b1010_1111
	db 0x00

global _gdt_kernel_data_segment
_gdt_kernel_data_segment: equ $ - gdt_start

	; data descriptor
	dw 0xffff
	dw 0x0000

	db 0x00
	db 0b10010010
	db 0b1100_1111
	db 0x00

gdt_end:

gdt_ptr_offset:
	dw (gdt_end - gdt_start) - 1
	dq (gdt_start - OFFSET)

gdt_ptr:
	dw (gdt_end - gdt_start) - 1
	dq (gdt_start)




SECTION .bootstrap.text

[BITS 32]

CPUNotSupported: 
	; We can't continue because we need features that this CPU does not have.

	; If this computer has a VGA text buffer, we can easily print an error message to the screen.
	
	mov eax, 0xb8000 ; VGA text buffer

	and SI, 0xff
	or SI, 0x4e00 ; Yellow on red

	mov dword [eax], 0x4e724e45 ; Write the text "ER"
	mov word [eax+4], SI ; Write the error number

loopForever:
	hlt
	jmp loopForever


global bootstrap_entry 
bootstrap_entry:
	;  This is where our code first starts running on boot. 
	;  The bootloader set up a minimal environment for us, but we need to do everything else on our own.

	; In the bootstrap we set up the call stack, then we set up just enough paging structures to map the C 
	; kernel before we clean up after ourself, and call the C kernel.
	
	cli ; Ensure that all interrupts are disabled, just to be safe
	
	; Set up the stack (we don't have paging on yet, so we need to subtract the offset)
	mov esp, (stack_top-OFFSET) 

	; Push information from multiboot for the C kernel to use later
	push eax
	push ebx


	; Check that the CPU supports the CPUID instruction
	pushfd ; We have to go through the stack to access the flags register, for some reason.
	pop eax
	mov ebx, eax ; Save the original value of the flags register

	xor eax, (1 << 21) ; Flip the ID bit

	push eax ; write the new value to the register, then read the register again
	popfd
	pushfd
	pop eax


	mov SI, "1" ; print 1 on screen if this is the reason we stop

	xor eax, ebx		; Check if the toggled ID bit stuck. If it hasn't, the cpu doesn't support the CPUID instruction.
	jz CPUNotSupported	; CPUs without the CPUID instruction don't support 64-bit / long mode either, and the kernel is
						; 64-bit only, we can't continue.


	mov SI, "2" ; print 2 on screen if this is the reason we stop 

	; First we need to check if the CPU supports the CPUID instruction we need to check for long mode
	mov eax, 0x80000000 ; Get the highest CPUID function supported
    cpuid
    cmp eax, 0x80000001
    jb CPUNotSupported

	mov SI, "3" ; print 3 on screen if this is the reason we stop
 	
	; Now we can finally check for long mode itself
	mov eax, 0x80000001
    cpuid
    test edx, (1 << 29)
    jz CPUNotSupported


	; Set CR0.PG to 0 to ensure paging is disabled before we configure it
	mov eax, cr0
	and eax, ~(1 << 31) ; Unset PG bit
	mov cr0, eax

	
	; Set up the temporary identity mapping for the first 2 MiB of memory
	extern pml4_table
	extern pml3_table_highest
	extern pml2_table_kernel
	extern pml1_table_kernel
	
	mov ecx, 0 ; first 512GB of memory
	mov eax, (pml3_table_low - OFFSET)

	and eax, 0xFFFF_F000
	or eax, (PRESENT | WRITABLE)

	imul ecx, 8

	mov [(pml4_table - OFFSET) + ecx], eax


	mov ecx, 0 ; first GB of memory
	mov eax, (pml2_table_low - OFFSET)

	and eax, 0xFFFF_F000
	or eax, (PRESENT | WRITABLE)

	imul ecx, 8

	mov [(pml3_table_low - OFFSET) + ecx], eax


	mov ecx, 0 ; first 2MiB of memory
	mov eax, 0

	and eax, 0xFFFF_F000
	or eax, (PRESENT | WRITABLE | PAGESIZE)

	imul ecx, 8

	mov [(pml2_table_low - OFFSET) + ecx], eax




	; Set CR3 to the physical address of the PML4 table
	mov eax, (pml4_table - OFFSET)
	mov cr3, eax

	; Set CR4.PAE to enable PAE, which is required for long-mode. We also set CR4.PSE to enable large pages
	mov eax, cr4
	or  eax, (1 << 4) | (1 << 5) ; Set PSE and PAE bit
	mov cr4, eax

	; Set EFER.LME to 1 to enable long mode and EFER.NXE to 1 to enable no-execute bit
	mov ecx, 0xC0000080
	rdmsr
	or  eax, (1 << 8) | (1 << 11) ; set LME bit and NXE bits
	wrmsr
 
	; Set CR0.PG to 1 to enable paging and CR0.WP to 1 to make write protect apply to supervisor mode too
	mov eax, cr0
	or  eax, (1 << 31) | (1 << 16)  ; set PG and WP bits
	mov cr0, eax
	

	; Paging and long mode should be enabled now, but we're still in 32-bit compatibility mode

	lgdt [gdt_ptr_offset - OFFSET]

	jmp _gdt_kernel_code_segment:bootstrap_low64 ; Jump to the 64-bit code segment to exit compatibility mode
	

[BITS 64] ; All code after here is 64-bit


bootstrap_low64:
	; Set the long mode segment registers
	mov ax, _gdt_kernel_data_segment
	mov ss, ax
	mov ds, ax
	mov es, ax

	; These two aren't really necessary, but we'll set them anyway
	mov fs, ax
	mov gs, ax

	mov rdx, 0x7fffffff_fffffe00 ; Address bitmask

	; map the first two megabytes of memory to the top of the address space

	mov ecx, 511 ; last 512 GB of memory
	mov rax, (pml3_table_highest - OFFSET)

	and rax, rdx
	or rax, (PRESENT | WRITABLE)

	imul ecx, 8

	mov [(pml4_table - OFFSET) + ecx], rax


	mov ecx, 510 ; second to last GiB of memory
	mov rax, (pml2_table_kernel - OFFSET)

	and rax, rdx
	or rax, (PRESENT | WRITABLE)

	imul ecx, 8

	mov [(pml3_table_highest - OFFSET) + ecx], rax
	

	mov ecx, 0 ; first 2MiB of the second to last GiB memory
	mov rax, (pml1_table_kernel - OFFSET)

	and rax, rdx
	or rax, (PRESENT | WRITABLE)

	imul ecx, 8

	mov [(pml2_table_kernel - OFFSET) + ecx], rax


	mov ecx, 0
	mov rax, 0x100000

	fillPML1:
		and rax, rdx
		or rax, (PRESENT | WRITABLE)
		mov [(pml1_table_kernel - OFFSET) + ecx], rax

		add ecx, 8
		add rax, 0x1000

		cmp ecx, 512*8
		jne fillPML1
	

	; Reload cr3 to flush the TLB
	mov rax, cr3
	mov cr3, rax


	; Jump to the high portion of the bootstrap, where we remove the identity mapping and jump to the C kernel
	mov rax, bootstrap_high64
	jmp rax


SECTION .text
extern kmain

bootstrap_high64:
	mov rdx, OFFSET
	add rsp, rdx ; Switch to accessing the stack using the high mapping

	lgdt [gdt_ptr] ; Switch to accessing the gdt using the high mapping


	; Remove the first megabyte identity mapping
	mov rax, 0
	mov [pml4_table+rax*4], rax

	; Reload cr3 to flush the TLB
	mov rax, cr3
	mov cr3, rax


	mov rbx, 0

	; Pop the multiboot information from the stack
	pop rax

	mov ebx, eax
	shr rax, 32
	
	; Per the system V AMD64 ABI, the first two arguments to a function are passed in RDI and RSI
	mov rdi, rax
	mov rsi, rbx

	; Call the C kernel
	call kmain

	; If the C kernel returns, then halt the system
kernelReturn:
	hlt
	jmp kernelReturn




SECTION .bss
ALIGN 16
stack_bottom:
	resb 0x1000 ; Allocate a 4 KiB stack
stack_top:

ALIGN 4096

; For the temporary identity mapping
pml3_table_low:
	resq 512
pml2_table_low:
	resq 512