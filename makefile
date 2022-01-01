NAME=tOS

_TARGETS=bootstrap.o ctype.o display.o gdt.o gdt_asm.o idt.o idt_asm.o io.o irq.o irq_asm.o isrs.o isrs_asm.o keyboard.o main.o memory.o mouse.o multiboot.o paging.o paging_asm.o pcspeaker.o pit.o rtc.o serial.o stdio.o string.o system.o time.o
TARGETS=$(patsubst %,out/obj/%,$(_TARGETS))

CC = Toolchain/i686-elf-cross/bin/i686-elf-gcc
LD = Toolchain/i686-elf-cross/bin/i686-elf-gcc

AS = nasm
QEMU = qemu-system-i386

override QEMUARGS := -boot d -cdrom out/$(NAME).iso -debugcon stdio -d cpu_reset,guest_errors -m 2048M -soundhw pcspk -rtc base=localtime -name $(NAME) ${QEMUARGS}#-serial file:serial.log
QEMUDEBUG = -s -S

#-c: Compile only, disable linking -ffreestanding: Assume non-hosted environment, -fstack-protector-strong: enable stack-smashing detection,
#-I src: Set include path, -std=gnu99: Use GNU C99 standard for compilation, -W*: Enable various warnings, -g: include debug symbols, -Og, optimize for debugging
COPT = -g\
-Og
override CFLAGS := -c\
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
$(COPT)\
-DGIT_VERSION="\"$(shell git describe --dirty --always --tags)\""\
-DCC_VERSION="\"$(shell $(CC) --version | head -n 1)\""\
$(CFLAGS)


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
	

out/$(NAME).bin: out/obj $(TARGETS) linker.ld
	$(LD) $(TARGETS) -T linker.ld $(LDFLAGS) -o out/$(NAME).bin

	@if ! grub-file --is-x86-multiboot out/$(NAME).bin; then \
		echo "The linked kernel is not multiboot compliant! The make process cannot proceed until this is fixed."; \
		rm out/$(NAME).bin; \
		false; \
	else \
		echo "The linked kernel is multiboot compliant."; \
	fi


isodir/boot/$(NAME).bin: out/$(NAME).bin isodir/boot/grub
	cp out/$(NAME).bin isodir/boot/$(NAME).bin

isodir/boot/grub/grub.cfg: isodir/boot/grub
	@echo 'set default="0"\nset timeout="1"\nmenuentry "$(NAME)" {\n\tmultiboot /boot/$(NAME).bin Hello\n}' > isodir/boot/grub/grub.cfg

out/$(NAME).iso: isodir/boot/$(NAME).bin isodir/boot/grub/grub.cfg
	grub-mkrescue -o out/$(NAME).iso isodir --compress gz --quiet -volid "$(NAME) Boot Disk"


run: out/$(NAME).iso
	$(QEMU) $(QEMUARGS)

debug: out/$(NAME).iso
	x-terminal-emulator -e "$(QEMU) $(QEMUDEBUG) $(QEMUARGS)"
	gdb -iex "file out/$(NAME).bin" -iex "target remote 127.0.0.1:1234" 

