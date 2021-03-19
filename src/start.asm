[BITS 32]

ALIGN 4
mboot:
	MULTIBOOT_PAGE_ALIGN    equ 1<<0    ; Load kernel and modules on a page boundary
	MULTIBOOT_MEMORY_INFO equ 1<<1 ; Provide your kernel with memory info
	MULTIBOOT_VIDEO_ENBABLE equ 1<<2 ; Provide a vbuffer
	MULTIBOOT_AOUT_KLUDGE equ 1<<16
	MULTIBOOT_HEADER_MAGIC  equ 0x1BADB002 ; Multiboot Magic value
	MULTIBOOT_HEADER_FLAGS  equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_VIDEO_ENBABLE | MULTIBOOT_AOUT_KLUDGE
	MULTIBOOT_CHECKSUM      equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
	EXTERN text, bss, KERNEL_END, KERNEL_START

	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_CHECKSUM

	dd mboot
	dd text
	dd bss
	dd KERNEL_END
	dd start

	dd 0 ; Mode type
	dd 800 ; width
	dd 600 ; height
	dd 32 ; depth

global start
start:
	mov esp, _sys_stack
	push ebx
	push eax
	extern kmain
	call kmain
	jmp $

; This will set up our new segment registers. We need to do
; something special in order to set CS. We do what is called a
; far jump. A jump that includes a segment as well as an offset.
; This is declared in C as 'extern void gdt_flush();'
global gdt_flush     ; Allows the C code to link to this
extern gp            ; Says that '_gp' is in another file
gdt_flush:
	lgdt [gp]        ; Load the GDT with our '_gp' which is a special pointer
	mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:flush2   ; 0x08 is the offset to our code segment: Far jump!
flush2:
	ret               ; Returns back to the C code!

global idt_load
extern idtp
idt_load:
	lidt [idtp]
	ret



; In just a few pages in this tutorial, we will add our Interrupt
; Service Routines (ISRs) right here!
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

;  0: Divide By Zero Exception
isr0:
	cli
	push byte 0    ; A normal ISR stub that pops a dummy error code to keep a
				   ; uniform stack frame
	push byte 0
	jmp isr_common_stub

;  1: Debug Exception
isr1:
	cli
	push byte 0
	push byte 1
	jmp isr_common_stub

;  2: NMI
isr2:
	cli
	push byte 0
	push byte 2
	jmp isr_common_stub

;  3: Breakpoint
isr3:
	cli
	push byte 0
	push byte 3
	jmp isr_common_stub

;  4: Into Detected Breakpoint
isr4:
	cli
	push byte 0
	push byte 4
	jmp isr_common_stub

;  5: Out of Bounds
isr5:
	cli
	push byte 0
	push byte 5
	jmp isr_common_stub

;  6: Debug Exception
isr6:
	cli
	push byte 0
	push byte 6
	jmp isr_common_stub

;  7: No Coprocessor
isr7:
	cli
	push byte 0
	push byte 7
	jmp isr_common_stub

;  8: Double Fault (With Error Code!)
isr8:
	cli
	push byte 8
	jmp isr_common_stub

;  9: Coprocessor Segment Overrun
isr9:
	cli
	push byte 0
	push byte 9
	jmp isr_common_stub

;  10: Bad TSS
isr10:
	cli
	push byte 10
	jmp isr_common_stub

;  11: Segment Not Present
isr11:
	cli
	push byte 11
	jmp isr_common_stub

;  12: Stack Fault
isr12:
	cli
	push byte 12
	jmp isr_common_stub

;  13: General Protection Fault
isr13:
	cli
	push byte 13
	jmp isr_common_stub

;  14: Page Fault
isr14:
	cli
	push byte 14
	jmp isr_common_stub

;  15: Unknown Interupt
isr15:
	cli
	push byte 0
	push byte 15
	jmp isr_common_stub

;  16: Coprocessor Fault
isr16:
	cli
	push byte 0
	push byte 16
	jmp isr_common_stub

;  17: Alignment Check (486+)
isr17:
	cli
	push byte 0
	push byte 17
	jmp isr_common_stub

;  18: Machine Check (Pentium / 586+)
isr18:
	cli
	push byte 0
	push byte 18
	jmp isr_common_stub

;19: Reserved
isr19:
	cli
	push byte 0
	push byte 19
	jmp isr_common_stub

;20: Reserved
isr20:
	cli
	push byte 0
	push byte 20
	jmp isr_common_stub

;21: Reserved
isr21:
	cli
	push byte 0
	push byte 21
	jmp isr_common_stub

;22: Reserved
isr22:
	cli
	push byte 0
	push byte 22
	jmp isr_common_stub

;23: Reserved
isr23:
	cli
	push byte 0
	push byte 23
	jmp isr_common_stub

;24: Reserved
isr24:
	cli
	push byte 0
	push byte 24
	jmp isr_common_stub

;25: Reserved
isr25:
	cli
	push byte 0
	push byte 25
	jmp isr_common_stub

;26: Reserved
isr26:
	cli
	push byte 0
	push byte 26
	jmp isr_common_stub

;27: Reserved
isr27:
	cli
	push byte 0
	push byte 27
	jmp isr_common_stub

;28: Reserved
isr28:
	cli
	push byte 0
	push byte 28
	jmp isr_common_stub

;29: Reserved
isr29:
	cli
	push byte 0
	push byte 29
	jmp isr_common_stub

;30: Reserved
isr30:
	cli
	push byte 0
	push byte 30
	jmp isr_common_stub

;31: Reserved
isr31:
	cli
	push byte 0
	push byte 31
	jmp isr_common_stub



; We call a C function in here. We need to let the assembler know
; that '_fault_handler' exists in another file
extern fault_handler

; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
isr_common_stub:
	pusha
	push ds
	push es
	push fs
	push gs
	mov ax, 0x10   ; Load the Kernel Data Segment descriptor!
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov eax, esp   ; Push us the stack
	push eax
	mov eax, fault_handler
	call eax       ; A special call, preserves the 'eip' register
	pop eax
	pop gs
	pop fs
	pop es
	pop ds
	popa
	add esp, 8     ; Cleans up the pushed error code and pushed ISR number
	iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!
			
global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

; 32: IRQ0
irq0:
	cli
	push byte 0
	push byte 32
	jmp irq_common_stub
; 33: IRQ1
irq1:
	cli
	push byte 0
	push byte 33
	jmp irq_common_stub
; 34: IRQ2
irq2:
	cli
	push byte 0
	push byte 34
	jmp irq_common_stub
; 35: IRQ3
irq3:
	cli
	push byte 0
	push byte 35
	jmp irq_common_stub
; 36: IRQ4
irq4:
	cli
	push byte 0
	push byte 36
	jmp irq_common_stub
; 37: IRQ5
irq5:
	cli
	push byte 0
	push byte 37
	jmp irq_common_stub
; 38: IRQ6
irq6:
	cli
	push byte 0
	push byte 38
	jmp irq_common_stub
; 39: IRQ7
irq7:
	cli
	push byte 0
	push byte 39
	jmp irq_common_stub
; 40: IRQ8
irq8:
	cli
	push byte 0
	push byte 40
	jmp irq_common_stub
; 41: IRQ9
irq9:
	cli
	push byte 0
	push byte 41
	jmp irq_common_stub
; 42: IRQ10
irq10:
	cli
	push byte 0
	push byte 42
	jmp irq_common_stub
; 43: IRQ11
irq11:
	cli
	push byte 0
	push byte 43
	jmp irq_common_stub
; 44: IRQ12
irq12:
	cli
	push byte 0
	push byte 44
	jmp irq_common_stub
; 45: IRQ13
irq13:
	cli
	push byte 0
	push byte 45
	jmp irq_common_stub
; 46: IRQ14
irq14:
	cli
	push byte 0
	push byte 46
	jmp irq_common_stub
; 47: IRQ15
irq15:
	cli
	push byte 0
	push byte 47
	jmp irq_common_stub

extern irq_handler

; This is a stub that we have created for IRQ based ISRs. This calls
; '_irq_handler' in our C code. We need to create this in an 'irq.c'
irq_common_stub:
	pusha
	push ds
	push es
	push fs
	push gs
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov eax, esp
	push eax
	mov eax, irq_handler
	call eax
	pop eax
	pop gs
	pop fs
	pop es
	pop ds
	popa
	add esp, 8
	iret

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
    or  ebx, 0x80004000 ; set PG and WP
    mov cr0, ebx        ; update cr0
    popa
	ret

SECTION .bss
	;resb 8192
	resb 0x10000
_sys_stack: