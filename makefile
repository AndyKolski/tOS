NAME := tOS
ARCH := i386

OSDIR := $(shell pwd)

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
QEMU := qemu-system-i386

REQUIRED_BINS := $(AS) $(QEMU) $(LD) $(CC)
$(foreach bin,$(REQUIRED_BINS),\
    $(if $(shell command -v $(bin) 2> /dev/null),,$(error Please install `$(bin)`. Try running the script Toolchain/build.sh)))


override QEMUARGS := -boot d -cdrom out/$(NAME).iso -debugcon stdio -d cpu_reset,guest_errors -m 2048M -soundhw pcspk -rtc base=localtime -name $(NAME) ${QEMUARGS}#-serial file:serial.log
QEMUDEBUG = -s -S


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


#-c: Compile only, disable linking -ffreestanding: Assume non-hosted environment, -fstack-protector-strong: enable stack-smashing detection,
#-I src: Set include path, -std=gnu99: Use GNU C99 standard for compilation, -g: include debug symbols, -Og, optimize for debugging
COPT = -g\
-Og
override CFLAGS := -c\
-ffreestanding\
-fstack-protector-strong\
-I src/kernel\
-I src/lib\
-std=gnu99\
-D__GIT_VERSION="\"$(shell git describe --dirty --always --tags)\""\
-D__CC_VERSION="\"$(shell $(CC) --version | head -n 1)\""\
-D__ARCH="\"$(ARCH)\""\
-MMD\
$(COPT)\
$(CWARNINGFLAGS)\
$(CFLAGS)

# -nostdlib: don't include standard libraries -lgcc: link libgcc
LDFLAGS = -nostdlib -lgcc
ASFLAGS = -felf


.PHONY: all iso clean run debug dumpvars info

info:
	@echo "To build and run $(NAME), use the following command: make run"

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

out/obj/kernel/arch/$(ARCH):
	mkdir out/obj/kernel/arch/$(ARCH) -p
mkdir out/obj/lib:
	mkdir out/obj/lib -p
isodir/boot/grub:
	mkdir isodir/boot/grub -p


out/obj/kernel/%.o: src/kernel/%.c
	@echo Kernel Compile: $@
	@$(CC) -o $@ $< $(CFLAGS)

out/obj/kernel/%.o: src/kernel/%.asm
	@echo Kernel Assemble: $@
	@$(AS) -o $@ $< $(ASFLAGS) 

out/obj/lib/%.o: src/lib/%.c
	@echo Lib Compile: $@
	@$(CC) -o $@ $< $(CFLAGS)

	

out/$(NAME).bin: out/obj/kernel/arch/$(ARCH) out/obj/lib $(ALLOBJS) linker.ld
	@echo Linking: $@
	@$(LD) $(ALLOBJS) -T linker.ld $(LDFLAGS) -o $@

	@if ! grub-file --is-x86-multiboot $@; then \
		echo "The linked kernel is not multiboot compliant! The make process cannot proceed until this is fixed"; \
		rm $@; \
		false; \
	else \
		echo "The linked kernel is multiboot compliant"; \
	fi


isodir/boot/$(NAME).bin: out/$(NAME).bin isodir/boot/grub
	cp $< $@

isodir/boot/grub/grub.cfg: isodir/boot/grub
	@echo 'set default="0"\nset timeout="1"\nmenuentry "$(NAME)" {\n\tmultiboot /boot/$(NAME).bin Hello\n}' > $@

out/$(NAME).iso: isodir/boot/$(NAME).bin isodir/boot/grub/grub.cfg
	grub-mkrescue -o $@ isodir --compress gz --quiet -volid "$(NAME) Boot Disk"


run: out/$(NAME).iso
	$(QEMU) $(QEMUARGS)

startdebugvm: out/$(NAME).iso
	x-terminal-emulator -e "$(QEMU) $(QEMUDEBUG) $(QEMUARGS)"

debug: startdebugvm
	gdb -iex "file out/$(NAME).bin" -iex "target remote 127.0.0.1:1234"
