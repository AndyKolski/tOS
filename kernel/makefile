include kernel/arch/$(ARCH)/makefile
KERNEL_OBJS := \
$(KERNEL_ARCH_OBJS)\
keyboard.o\
main.o\
memory.o\
mouse.o\
paging.o\
pcspeaker.o\
pit.o\
rtc.o\
serial.o\
system.o\
time.o\
display.o\
gdt.o\
idt.o\
intmath.o\
io.o\
irq.o\
isrs.o\
isrs_asm.o\
multiboot.o\
paging_asm.o\
bootstrap.o\
gdt_asm.o\
idt_asm.o\
irq_asm.o

KERNEL_OBJS := $(patsubst %.o,out/obj/kernel/%.o,$(KERNEL_OBJS))