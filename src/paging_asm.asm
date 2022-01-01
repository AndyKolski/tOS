[BITS 32]

global enablePaging
extern PageDirectoryTable
enablePaging:
	pusha
	mov eax, PageDirectoryTable
    mov cr3, eax

    mov ebx, cr4        ; read current cr4
    or  ebx, 0x00000010 ; set PSE
    mov cr4, ebx        ; update cr4

    mov ebx, cr0        ; read current cr0
    or  ebx, 0x80010000 ; set PG and WP
    mov cr0, ebx        ; update cr0
    popa
	ret