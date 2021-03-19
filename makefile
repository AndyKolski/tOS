NAME=tOS

_TARGETS=start.o main.o libs.o gdt.o idt.o isrs.o irq.o keyboard.o display.o memory.o pcspeaker.o pit.o serial.o io.o stdio.o string.o mouse.o rtc.o time.o paging.o
TARGETS=$(patsubst %,out/obj/%,$(_TARGETS))

CC = i686-elf-gcc
LD = i686-elf-gcc
AS = nasm

# -Wall: enable all warnings, -Wextra: enable extra warnings, -Werror: treat warnings as errors -I: set include path -c: disable linking -std=gnu99: use GNU C99 standard for compilation -g: include debug symbols -Og: optimize for debug
CFLAGS = -Wall -Wextra -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wshadow -Werror -I src -c -ffreestanding -std=gnu99 -g -Og
# -ffreestanding: assume non-hosted environment -nostdlib: don't include standard libraries -lgcc: link libgcc
LDFLAGS = -ffreestanding -nostdlib -lgcc
ASFLAGS = -felf

.PHONY: all iso clean run debug

all: out/$(NAME).bin
	@echo 'BIN done! located at: out/$(NAME).bin'

iso: out/$(NAME).iso
	@echo 'ISO done! located at: out/$(NAME).iso'

clean:
	rm -rf out isodir

out/obj:
	mkdir out/obj -p
isodir/boot/grub:
	mkdir isodir/boot/grub -p

out/obj/%.o: src/%.c src/%.h
	$(CC) -o $@ $< $(CFLAGS) 
out/obj/%.o: src/%.asm
	$(AS) -o $@ $< $(ASFLAGS) 
	


out/$(NAME).bin: out/obj $(TARGETS)
	$(LD) $(TARGETS) -T linker.ld $(LDFLAGS) -o out/$(NAME).bin
	@echo '$(NAME) compiled successfully!'

out/$(NAME).iso: isodir/boot/grub out/$(NAME).bin
	@echo 'Packing into ISO...'
	@cp out/$(NAME).bin isodir/boot/$(NAME).bin
	@echo 'set default="0"\nset timeout="1"\nmenuentry "$(NAME)" {\n\tmultiboot /boot/$(NAME).bin Hello\n}' > isodir/boot/grub/grub.cfg
	grub-mkrescue -o out/$(NAME).iso isodir --compress gz --quiet


run: out/$(NAME).iso
	@qemu-system-i386 -boot d -cdrom out/$(NAME).iso -debugcon stdio -d cpu_reset,guest_errors -m 2048M -soundhw pcspk -rtc base=localtime #-serial file:serial.log 

debug: out/$(NAME).iso
	gnome-terminal -e "qemu-system-i386 -s -S -boot d -cdrom out/$(NAME).iso -debugcon stdio -d cpu_reset,guest_errors -m 2048M -soundhw pcspk -rtc base=localtime" #-serial file:serial.log 
	gdb -iex "file out/$(NAME).bin" -iex "target remote 127.0.0.1:1234" 

