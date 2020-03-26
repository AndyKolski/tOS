NAME=tOS
SOURCES=start.o main.o common.o gdt.o idt.o isrs.o irq.o timer.o kb.o display.o memory.o pcspeaker.o pit.o serial.o
CARGS = -Wall -Wextra -g -Og -I src -c -ffreestanding # -Wall: enable all warnings -g: include debug symbols -Og: optimize for debug -I: include path -c: disable linking 
LARGS = -ffreestanding -nostdlib -lgcc # -ffreestanding: compile for non-hosted environment -nostdlib: don't include standard libraries -lgcc: include libgcc
COMPILER = i686-elf-gcc

make: clean $(SOURCES)
	@echo 'linking kernel files...'
	@$(COMPILER) -o out/$(NAME).bin $(SOURCES) -T linker.ld $(LARGS)
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
memory.o: src/memory.c
	@echo 'compiling memory.o ...'
	@$(COMPILER) $(CARGS) -o memory.o src/memory.c
	@echo 'done'
pcspeaker.o: src/pcspeaker.c
	@echo 'compiling pcspeaker.o ...'
	@$(COMPILER) $(CARGS) -o pcspeaker.o src/pcspeaker.c
	@echo 'done'
pit.o: src/pit.c
	@echo 'compiling pit.o ...'
	@$(COMPILER) $(CARGS) -o pit.o src/pit.c
	@echo 'done'
serial.o: src/serial.c
	@echo 'compiling serial.o ...'
	@$(COMPILER) $(CARGS) -o serial.o src/serial.c
	@echo 'done'

iso: make
	@echo 'Packing into ISO...'
	-mkdir isodir isodir/boot
	@cp out/$(NAME).bin isodir/boot/$(NAME).bin
	@echo 'set default="0"\nset timeout="1"\nmenuentry "$(NAME)" {\n\tmultiboot /boot/$(NAME).bin\n}' > isodir/boot/grub/grub.cfg
	@grub-mkrescue -o out/$(NAME).iso isodir
	@echo 'done'
	@echo 'ISO moved to out/$(NAME).iso'
run: iso
	@qemu-system-i386 -s -boot d -cdrom out/$(NAME).iso -debugcon stdio -d cpu_reset,guest_errors -m 2048M -soundhw pcspk #-serial file:serial.log 