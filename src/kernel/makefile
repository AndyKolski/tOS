include src/kernel/arch/$(ARCH)/makefile
KERNEL_OBJS := \
$(KERNEL_ARCH_OBJS)\
display.o\
interrupts/idt.o\
interrupts/irq_asm.o\
interrupts/irq.o\
interrupts/isrs_asm.o\
interrupts/isrs.o\
interrupts/pic.o\
formatting.o\
io.o\
keyboard.o\
main.o\
memory/paging.o\
memory/pmm.o\
memory/memregion.o\
memory/allocators/bump.o\
memory/allocators/bitmap.o\
mouse.o\
multiboot_header_asm.o\
multibootdata.o\
pcspeaker.o\
pci/pci.o\
serial.o\
system.o\
time/pit.o\
time/rtc.o\
time/time.o\

KERNEL_OBJS := $(patsubst %.o,out/obj/kernel/%.o,$(KERNEL_OBJS))