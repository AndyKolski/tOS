PRESENT equ 1<<0
WRITABLE equ 1<<1
USERACCESS equ 1<<2
WRITETHROUGHCACHE equ 1<<3
DISABLECACHE equ 1<<4
ACCESSED equ 1<<5
DIRTY equ 1<<6
FOURMIBPAGE equ 1<<7

[BITS 32]

SECTION .bootstrap.text

global bootstrap_entry
bootstrap_entry:
	mov esp, stack_top-0xC0000000
	add ebx, 0xC0000000
	push ebx
	push eax


	extern PageDirectoryTable
	

	; clear out the PDT
	mov eax, 0
	fillPDT:
		mov ebx, 0

		mov [PageDirectoryTable+eax*4-0xC0000000], ebx

		inc eax

		cmp eax, 1024
		jne fillPDT ; If eax is 1024, continue. Otherwise loop again

	; temporarily identify map the first megabyte so this code keeps working once we enable paging

	mov eax, 0 >> 22 ; virtual address
	mov ebx, 0 ; physical address

	shl ebx, 22

	or ebx, (PRESENT|WRITABLE|WRITETHROUGHCACHE|FOURMIBPAGE)
	mov [PageDirectoryTable+eax*4-0xC0000000], ebx

	; map 0xC0000000 to the first 4 megabytes for the actual kernel

	mov eax, 0xC0000000 >> 22 ; virtual address
	mov ebx, 0 ; physical address

	shl ebx, 22

	or ebx, (PRESENT|WRITABLE|WRITETHROUGHCACHE|FOURMIBPAGE)
	mov [PageDirectoryTable+eax*4-0xC0000000], ebx


	; configure and enable paging

	mov eax, (PageDirectoryTable-0xC0000000)
    mov cr3, eax

    mov ebx, cr4        ; read current cr4
    or  ebx, 0x00000010 ; set PSE
    mov cr4, ebx        ; update cr4

    mov ebx, cr0        ; read current cr0
    or  ebx, 0x80010000 ; set PG and WP
    mov cr0, ebx        ; update cr0

	add esp, 0xC0000000 ; switch to accessing the stack using the higher-half mapping

	jmp high_jump

SECTION .text

high_jump:
	; remove first megabyte identity mapping
	mov eax, 0
	mov [PageDirectoryTable+eax*4], eax

	; reload cr3 to force the cpu to reload the page directory table
	mov eax, cr3
	mov cr3, eax

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
resb 16384
stack_top: