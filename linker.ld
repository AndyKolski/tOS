OUTPUT_FORMAT("elf32-i386")
ENTRY(start)

SECTIONS {
	. = 0x00100000;
	KERNEL_START = .;
	.text ALIGN (0x1000) :
	{
		text = .;
		*(.text)
	}

	.rodata ALIGN (0x1000) :
	{
		*(.rodata)
	}

	.data ALIGN (0x1000) :
	{
		*(.data)
	}

	.bss ALIGN (0x1000) :
	{
		bss = .;
		*(COMMON)
		*(.bss)
	}
	KERNEL_END = .;
}