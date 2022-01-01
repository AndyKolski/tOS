SECTION .multiboot
ALIGN 4

mboot:
	MULTIBOOT_PAGE_ALIGN    equ 1<<0    ; Load kernel and modules on a page boundary
	MULTIBOOT_MEMORY_INFO equ 1<<1 ; Provide your kernel with memory info
	MULTIBOOT_VIDEO_ENBABLE equ 1<<2 ; Provide a vbuffer
	MULTIBOOT_AOUT_KLUDGE equ 1<<16
	MULTIBOOT_HEADER_MAGIC  equ 0x1BADB002 ; Multiboot Magic value
	MULTIBOOT_HEADER_FLAGS  equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_VIDEO_ENBABLE | MULTIBOOT_AOUT_KLUDGE
	MULTIBOOT_CHECKSUM      equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
	EXTERN text, bss, KERNEL_END, KERNEL_START, bootstrap_entry

	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_CHECKSUM

	dd mboot
	dd text
	dd bss
	dd KERNEL_END
	dd bootstrap_entry

	dd 0 ; Mode type
	dd 800 ; width
	dd 600 ; height
	dd 32 ; depth