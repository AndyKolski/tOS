NAME=tOS
SOURCES=start.o main.o common.o gdt.o idt.o isrs.o irq.o timer.o kb.o display.o
CARGS = -Wall -g -Og -ffreestanding -I src -c
COMPILER = ~/gcc-cross-compiler/PKG/usr/local/cross/ia32/bin/i686-pc-linux-gnu-gcc
LINKER = ~/gcc-cross-compiler/PKG/usr/local/cross/ia32/bin/i686-pc-linux-gnu-ld

make: $(SOURCES)
	@echo 'linking kernel files...'
	@$(LINKER) -T linker.ld -o out/$(NAME).bin $(SOURCES)
	@echo 'done'
	@echo 'Bin moved to out/$(NAME).bin'

clean:
	-rm -r $(SOURCES)

start.o: src/start.asm
	@echo 'compiling start.o ...'
	@nasm -felf src/start.asm -o start.o
	@echo 'done'
main.o: src/main.c
	@echo 'compiling main.o ...'
	@$(COMPILER) $(CARGS) -o main.o src/main.c
	@echo 'done'
common.o: src/common.c
	@echo 'compiling common.o ...'
	@$(COMPILER) $(CARGS) -o common.o src/common.c
	@echo 'done'
scrn.o: src/scrn.c
	@echo 'compiling scrn.o ...'
	@$(COMPILER) $(CARGS) -o scrn.o src/scrn.c
	@echo 'done'
gdt.o: src/gdt.c
	@echo 'compiling gdt.o ...'
	@$(COMPILER) $(CARGS) -o gdt.o src/gdt.c
	@echo 'done'
idt.o: src/idt.c
	@echo 'compiling idt.o ...'
	@$(COMPILER) $(CARGS) -o idt.o src/idt.c
	@echo 'done'
isrs.o: src/isrs.c
	@echo 'compiling isrs.o ...'
	@$(COMPILER) $(CARGS) -o isrs.o src/isrs.c
	@echo 'done'
irq.o: src/irq.c
	@echo 'compiling irq.o ...'
	@$(COMPILER) $(CARGS) -o irq.o src/irq.c
	@echo 'done'
timer.o: src/timer.c
	@echo 'compiling timer.o ...'
	@$(COMPILER) $(CARGS) -o timer.o src/timer.c
	@echo 'done'
kb.o: src/kb.c
	@echo 'compiling kb.o ...'
	@$(COMPILER) $(CARGS) -o kb.o src/kb.c
	@echo 'done'
display.o: src/display.c
	@echo 'compiling display.o ...'
	@$(COMPILER) $(CARGS) -o display.o src/display.c
	@echo 'done'

iso: make
	@echo 'Packing into ISO...'
	-mkdir isodir isodir/boot
	@cp out/$(NAME).bin isodir/boot/$(NAME).bin
	@echo 'set default="0"\nset timeout="0"\nmenuentry "$(NAME)" {\n\tmultiboot /boot/$(NAME).bin\n}' > isodir/boot/grub/grub.cfg
	@grub-mkrescue -o out/$(NAME).iso isodir
	@echo 'done'
	@echo 'ISO moved to out/$(NAME).iso'
run: iso
	@qemu-system-i386 -s -boot d -cdrom out/$(NAME).iso -d guest_errors -serial file:serial.log -m 2048M