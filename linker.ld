OUTPUT_FORMAT("elf32-i386")
ENTRY(start)

_KERNEL_START_ = 0x00100000;

SECTIONS
{
  .text (_KERNEL_START_) : {
    code = .;
    __START_OF_KERNEL = .;
    *(.text)
    *(.rodata)
  }

  .data : ALIGN(0x1000) {
    data = .;
    *(.data)
  }

  .bss : ALIGN(0x1000) {
    bss = .;
    *(.bss)
  }
  end = .;
  __END_OF_KERNEL = .;
}