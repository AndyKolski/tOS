NAME=tOS

_TARGETS=start.o display.o gdt.o idt.o io.o irq.o isrs.o keyboard.o libs.o main.o memory.o mouse.o paging.o pcspeaker.o pit.o rtc.o serial.o stdio.o string.o time.o
TARGETS=$(patsubst %,out/obj/%,$(_TARGETS))

CC = i686-elf-gcc
LD = i686-elf-gcc
AS = nasm

#-c: Compile only, disable linking -ffreestanding: Assume non-hosted environment, -fstack-protector-strong: enable stack-smashing detection,
#-I src: Set include path, -std=gnu99: Use GNU C99 standard for compilation, -W*: Enable various warnings, -g: include debug symbols, -Og, optimize for debugging
CFLAGS = -c\
-ffreestanding\
-fstack-protector-strong\
-I src\
-std=gnu99\
-Wall\
-Wextra\
-Wduplicated-branches\
-Wduplicated-cond\
-Wlogical-op\
-Wnull-dereference\
-Wredundant-decls\
-Wshadow\
-Wsign-conversion\
-Werror\
-g\
-Og

# -nostdlib: don't include standard libraries -lgcc: link libgcc
LDFLAGS = -nostdlib -lgcc
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

