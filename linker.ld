OUTPUT_FORMAT("elf32-i386")
ENTRY(bootstrap_entry)

SECTIONS {
	. = 0x00100000;

	.multiboot ALIGN (4) :
	{
		out/obj/multiboot.o(.multiboot)
	}

	.bootstrap.text ALIGN(4K) : {
		out/obj/bootstrap.o(.bootstrap.text)
	}

	. += 0xC0000000;

	KERNEL_START = .;

	.text ALIGN (4K) : AT(ADDR(.text) - 0xC0000000)
	{
		*(.text)
	}

	.rodata ALIGN (4K) : AT(ADDR(.rodata) - 0xC0000000)
	{
		*(.rodata)
	}

	.data ALIGN (4K) : AT(ADDR(.data) - 0xC0000000)
	{
		*(.data)
	}

	.bss ALIGN (4K) : AT(ADDR(.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
	}
	KERNEL_END = .;
}