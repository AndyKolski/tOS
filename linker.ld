OUTPUT_FORMAT("elf64-x86-64")
ENTRY(bootstrap_entry)

__BOOTLOADER_LOAD_ADDR = 0x100000;

/* Kernel at 0xffffffff80000000 */
__OFFSET = (0xFFFFFFFF80000000 - __BOOTLOADER_LOAD_ADDR); 

SECTIONS {
	. = __BOOTLOADER_LOAD_ADDR;

	.multiboot ALIGN (8) : {
		*(.multiboot)
	}

	.bootstrap.text : {
		*(.bootstrap.text)
	}

	/* We don't count the multiboot header or bootstrap code as part of the kernel as they aren't needed once paging is set up */
	__KERNEL_START = .;
	. += __OFFSET;

	.text ALIGN (4K) : AT(ADDR(.text) - __OFFSET) {
		__TEXT_START = .;
		*(.text)
	}
	__TEXT_END = .;

	.rodata ALIGN (4K) : AT(ADDR(.rodata) - __OFFSET) {
		__RODATA_START = .;
		*(.rodata .rodata.*)
	}
	__RODATA_END = .;

	.data ALIGN (4K) : AT(ADDR(.data) - __OFFSET) {
		__DATA_START = .;
		*(.data .data.*)
	}
	__DATA_END = .;

	.bss ALIGN (4K) : AT(ADDR(.bss) - __OFFSET) {
		__BSS_START = .;
		*(.bss .bss.* .common)
		*(COMMON)
	}
	__BSS_END = .;
	__KERNEL_END = ALIGN (4K) - __OFFSET;
}
