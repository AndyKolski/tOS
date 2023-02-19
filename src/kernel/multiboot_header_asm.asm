MULTIBOOT_HEADER_MAGIC  equ 0xe85250d6 ; Multiboot Magic value
MULTIBOOT_ARCHITECTURE  equ 0 ; Architecture (x86 32-bit protected mode)
MULTIBOOT_HEADER_LENGTH equ (mboot_end - mboot) ; Header length

SECTION .multiboot

mboot:
	ALIGN 8

	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_ARCHITECTURE
	dd MULTIBOOT_HEADER_LENGTH

	dd -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE + MULTIBOOT_HEADER_LENGTH) ; Checksum

	; Tags

	dw 5	; Type (framebuffer)
	dw 0	; Flags
	dd 20	; Size
	dd 800	; Width
	dd 600	; Height
	dd 32	; Depth

	ALIGN 8

	dw 0	; Type
	dw 0	; Flags
	dd 8	; Size
mboot_end: