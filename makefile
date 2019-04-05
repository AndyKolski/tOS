NAME=tOS
SOURCES=start.o main.o scrn.o gdt.o idt.o isrs.o irq.o timer.o kb.o display.o
CARGS = -Wall -O3 -nostdinc -ffreestanding -I src -c

make: clean $(SOURCES)
	@mkdir out
	@echo 'linking kernel files...'
	@i686-elf-ld -T linker.ld -o out/$(NAME).bin $(SOURCES)
	@echo 'done'
	@echo 'Bin moved to out/$(NAME).bin'

clean:
	-rm -r $(SOURCES) out

start.o:
	@echo 'compiling start.o ...'
	@nasm -felf src/start.asm -o start.o
	@echo 'done'

main.o:
	@echo 'compiling main.o ...'
	@i686-elf-gcc $(CARGS) -o main.o src/main.c
	@echo 'done'
scrn.o:
	@echo 'compiling scrn.o ...'
	@i686-elf-gcc $(CARGS) -o scrn.o src/scrn.c
	@echo 'done'
gdt.o:
	@echo 'compiling gdt.o ...'
	@i686-elf-gcc $(CARGS) -o gdt.o src/gdt.c
	@echo 'done'
idt.o:
	@echo 'compiling idt.o ...'
	@i686-elf-gcc $(CARGS) -o idt.o src/idt.c
	@echo 'done'
isrs.o:
	@echo 'compiling isrs.o ...'
	@i686-elf-gcc $(CARGS) -o isrs.o src/isrs.c
	@echo 'done'
irq.o:
	@echo 'compiling irq.o ...'
	@i686-elf-gcc $(CARGS) -o irq.o src/irq.c
	@echo 'done'
timer.o:
	@echo 'compiling timer.o ...'
	@i686-elf-gcc $(CARGS) -o timer.o src/timer.c
	@echo 'done'
kb.o:
	@echo 'compiling kb.o ...'
	@i686-elf-gcc $(CARGS) -o kb.o src/kb.c
	@echo 'done'
display.o:
	@echo 'compiling display.o ...'
	@i686-elf-gcc $(CARGS) -o display.o src/display.c
	@echo 'done'



run: iso
	@qemu-system-i386 -boot d -cdrom out/$(NAME).iso -d guest_errors
iso: make
	@echo 'Packing into ISO...'
	-mkdir isodir isodir/boot
	-rm out/$(NAME).iso
	-rm isodir/boot/$(NAME).bin 
	@cp out/$(NAME).bin isodir/boot/$(NAME).bin
	@echo 'set default="0"\nset timeout="0"\nmenuentry "$(NAME)" {\n\tmultiboot /boot/$(NAME).bin\n}' > isodir/boot/grub/grub.cfg
	@grub-mkrescue -o out/$(NAME).iso isodir
	@echo 'done'
	@echo 'ISO moved to out/$(NAME).iso'