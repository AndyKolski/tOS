OUTPUT_FORMAT("elf32-i386")
ENTRY(bootstrap_entry)

SECTIONS {
	. = 0x00100000;
	KERNEL_START = .;

	.multiboot ALIGN (4) :
	{
		*(.multiboot)
	}

	.text ALIGN (4K) :
	{
		text = .;
		*(.text)
	}

	.rodata ALIGN (4K) :
	{
		*(.rodata)
	}

	.data ALIGN (4K) :
	{
		*(.data)
	}

	.bss ALIGN (4K) :
	{
		bss = .;
		*(COMMON)
		*(.bss)
	}
	KERNEL_END = .;
}