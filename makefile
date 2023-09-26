NAME := tOS
ARCH := x86_64

OSDIR := $(shell pwd)

default: all

include src/kernel/makefile # for KERNEL_OBJS
include src/lib/makefile # for LIB_OBJS

ALLOBJS = $(KERNEL_OBJS) $(LIB_OBJS)

KERNEL_OBJDIR := $(OSDIR)out/obj/kernel/
LIB_OBJDIR := $(OSDIR)out/obj/lib/

DEPFILES:= $(patsubst %.o,%.d,$(ALLOBJS))

-include $(DEPFILES)

TOOLCHAINBIN := $(OSDIR)/Toolchain/$(ARCH)-elf-cross/bin/

CC := $(TOOLCHAINBIN)$(ARCH)-elf-gcc
LD := $(TOOLCHAINBIN)$(ARCH)-elf-gcc

AS := nasm
QEMU := qemu-system-$(ARCH)

REQUIRED_BINS := $(AS) $(QEMU) $(LD) $(CC)
$(foreach bin,$(REQUIRED_BINS),\
    $(if $(shell command -v $(bin) 2> /dev/null),,$(error Please install `$(bin)`. Try running the script Toolchain/build.sh)))

override QEMUARGS := -drive format=raw,media=cdrom,file=out/$(NAME).iso\
-boot d\
-debugcon stdio\
-m 2048M\
-audiodev pa,id=audio0\
-machine pcspk-audiodev=audio0\
-rtc base=localtime\
-name $(NAME)\
${QEMUARGS}\
#-serial file:serial.log #

# -gdb: listen for gdb connection on port 1234, -S: Start with the VM paused, -d: enable additional debug logging, -no-shutdown: don't exit on guest shutdown, -no-reboot: don't allow reboots (including triple faults)
QEMUDEBUG = -s\
-S\
-d cpu_reset,guest_errors,unimp\
-no-shutdown\
-no-reboot\


CWARNINGFLAGS := -Wall\
-Wextra\
-Wduplicated-branches\
-Wduplicated-cond\
-Wlogical-op\
-Wnull-dereference\
-Wredundant-decls\
-Wshadow\
-Wsign-conversion\
-Werror\
-Wcast-align\
-Winline\
-Wparentheses


# -g: include debug symbols, -Og: optimize for debugging, -c: Compile code, but don't link it yet, -ffreestanding: Assume non-hosted environment,
# -nostdlib: don't include standard libraries, -fstack-protector-strong: enable stack-smashing detection, -I src: Set include path, 
# -std=gnu99: Use GNU C99 standard for compilation, -MMD: create dependency files, -mcmodel=large: support placing code and data anywhere in the 64-bit address space,
# -mno-red-zone: disable the red zone, -mno-mmx: disable MMX instructions, -mno-sse: disable SSE instructions,
# -mno-sse2: disable SSE2 instructions, -funsigned-char: char is unsigned by default
COPT = -g\
-Og
override CFLAGS := -c\
-ffreestanding\
-nostdlib\
-fstack-protector-strong\
-I src/kernel\
-I src/lib\
-std=gnu99\
-D__GIT_VERSION="\"$(shell git describe --dirty --always --tags)\""\
-D__CC_VERSION="\"$(shell $(CC) --version | head -n 1)\""\
-D__ARCH="\"$(ARCH)\""\
-MMD\
-mcmodel=large\
-mno-red-zone -mno-mmx -mno-sse -mno-sse2\
-funsigned-char\
$(COPT)\
$(CWARNINGFLAGS)\
$(CFLAGS)

# -nostdlib: don't include standard libraries -lgcc: link libgcc
LDFLAGS = -nostdlib -lgcc -z max-page-size=0x1000
ASFLAGS = -felf64


.PHONY: all iso clean run debug dumpvars info


dumpvars:
	$(info $$CC is [${CC}])
	$(info $$LD is [${LD}])

	$(info $$DEPFILES is [${DEPFILES}])
	$(info $$KERNEL_OBJS is [${KERNEL_OBJS}])
	$(info $$LIB_OBJS is [${LIB_OBJS}])
	$(info $$CFLAGS is [${CFLAGS}])
	$(info $$OSDIR is [${OSDIR}])
	@true


all: out/$(NAME).bin
	@echo 'BIN done! located at: $<'

iso: out/$(NAME).iso
	@echo 'ISO done! located at: $<'

clean:
	rm -rf out isodir

out/obj/kernel:
	mkdir out/obj/kernel/arch/$(ARCH) -p
	mkdir out/obj/kernel/interrupts -p
	mkdir out/obj/kernel/memory -p
	mkdir out/obj/kernel/time -p
	mkdir out/obj/kernel/pci -p
mkdir out/obj/lib:
	mkdir out/obj/lib -p
	mkdir out/obj/lib/printf -p
isodir/boot/grub:
	mkdir isodir/boot/grub -p


out/obj/kernel/%.o: src/kernel/%.asm
	@echo Kernel Assemble: $@
	@$(AS) -o $@ $< $(ASFLAGS) 

out/obj/kernel/%.o: src/kernel/%.c
	@echo Kernel Compile: $@
	@$(CC) -o $@ $< $(CFLAGS)

out/obj/lib/%.o: src/lib/%.c
	@echo Lib Compile: $@
	@$(CC) -o $@ $< $(CFLAGS) $(LIBCFLAGS)

	

out/$(NAME).bin: out/obj/kernel out/obj/lib $(ALLOBJS) linker.ld
	@echo Linking: $@
	@$(LD) $(ALLOBJS) -T linker.ld $(LDFLAGS) -o $@

	@if ! grub-file --is-x86-multiboot2 $@; then \
		echo "The linked kernel is not multiboot compliant! The make process cannot proceed until this is fixed"; \
		# rm $@; \
		false; \
	else \
		echo "The linked kernel is multiboot compliant"; \
	fi


isodir/boot/$(NAME).bin: out/$(NAME).bin isodir/boot/grub
	cp $< $@

isodir/boot/grub/grub.cfg: isodir/boot/grub
	@echo -e 'set default="0"\nset timeout="1"\nmenuentry "$(NAME)" {\n\tmultiboot2 /boot/$(NAME).bin Hello\n}' > $@

out/$(NAME).iso: isodir/boot/$(NAME).bin isodir/boot/grub/grub.cfg
	grub-mkrescue -o $@ isodir --compress gz --quiet -volid "$(NAME) Boot Disk"


run: out/$(NAME).iso
	$(QEMU) $(QEMUARGS)

startdebugvm: out/$(NAME).iso
	killall $(QEMU) || true
	xterm -e "$(QEMU) $(QEMUDEBUG) $(QEMUARGS)" &


debug: startdebugvm
	gdb -iex "file out/$(NAME).bin" -iex "target remote 127.0.0.1:1234"
