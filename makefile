NAME=tOS
TARGETS=start.o main.o libs.o gdt.o idt.o isrs.o irq.o timer.o kb.o display.o memory.o pcspeaker.o pit.o serial.o io.o
CFLAGS = -Wall -Wextra -g -Og -I src -c -ffreestanding# -Wall: enable all warnings -g: include debug symbols -Og: optimize for debug -I: include path -c: disable linking 
LFLAGS = -ffreestanding -nostdlib -lgcc# -ffreestanding: compile for non-hosted environment -nostdlib: don't include standard libraries -lgcc: include libgcc
CC = i686-elf-gcc

all : setup out/$(NAME).bin
	@echo 'done'

setup:
	@mkdir -p out isodir isodir/boot isodir/boot/grub

out/$(NAME).bin : $(TARGETS)
	$(CC) $(TARGETS) -T linker.ld $(LFLAGS) -o out/$(NAME).bin

%.o : src/%.asm
	nasm $< -felf -o $@
	
%.o : src/%.c src/%.h
	$(CC) $< $(CFLAGS) -o $@

iso : all
	@echo 'Packing into ISO...'
	@cp out/$(NAME).bin isodir/boot/$(NAME).bin
	@echo 'set default="0"\nset timeout="1"\nmenuentry "$(NAME)" {\n\tmultiboot /boot/$(NAME).bin\n}' > isodir/boot/grub/grub.cfg
	@grub-mkrescue -o out/$(NAME).iso isodir
	@echo 'done'
	@echo 'ISO moved to out/$(NAME).iso'
run : iso
	@qemu-system-i386 -s -boot d -cdrom out/$(NAME).iso -debugcon stdio -d cpu_reset,guest_errors -m 2048M -soundhw pcspk #-serial file:serial.log 

clean :
	-rm $(TARGETS)