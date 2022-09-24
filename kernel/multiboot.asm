SECTION .multiboot
ALIGN 4

mboot:
	MULTIBOOT_PAGE_ALIGN    equ 1<<0    ; Load kernel and modules on a page boundary
	MULTIBOOT_MEMORY_INFO equ 1<<1 ; Provide your kernel with memory info
	MULTIBOOT_VIDEO_ENBABLE equ 1<<2 ; Provide a vbuffer
	MULTIBOOT_HEADER_MAGIC  equ 0x1BADB002 ; Multiboot Magic value
	MULTIBOOT_HEADER_FLAGS  equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_VIDEO_ENBABLE
	MULTIBOOT_CHECKSUM      equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_CHECKSUM

	; unnecessary fields for aout kludge

	dd 0 ; header_addr
	dd 0 ; load_addr
	dd 0 ; load_end_addr
	dd 0 ; bss_end_addr
	dd 0 ; entry_addr

	; video mode fields

	dd 0 ; Mode type
	dd 800 ; width
	dd 600 ; height
	dd 32 ; depth