# t/OS

A simple operating system for x86 systems, originally created to learn more about how x86 PCs work, and to get a better understanding of assembly, low-level code, and OS fundamentals.

## About

Currently, t/OS is fairly bare-bones, but it is being slowly added onto, so at some point it might even be able to do something almost useful.

### Features & Things

Currently t/OS has the following features:

- A graphical VESA terminal
- Full date and time support
- Interrupt and exception handling
- Kernel memory management
- Keyboard and mouse support
- Paging
- PC Speaker support
- Serial support
- Very experimental ring-3 (userland) functionality
- Probably some other things too

## Building & Running t/OS

### Dependencies
To build t/OS, you need to ensure that the following dependencies are installed.

- curl
- gcc
- grub-common
- make
- nasm
- qemu
- xorriso
- Probably a few other things that I have forgotten to write here.

On Debian/Ubuntu, they can be installed with the following command:
```console
sudo apt install curl gcc grub-common make nasm qemu xorriso
```
### Toolchain
You will also need to build the toolchain (cross-compiler). Thankfully, we have a script to do that for you.

Just run the following command:
```console
Toolchain/build.sh
```

It may take a while to finish, depending on your computer, so expect to wait somewhere between a few minutes to a few hours if your computer is slower.

### Actually building it 
Actually building t/OS is really simple.

To compile and run t/OS in a QEMU VM, just run `make run`

To compile t/OS to a multiboot compatible file, run `make all`, and the result will be in `out/tOS.bin`.
To get a bootable ISO file, run `make iso`, and the ISO will be in `out/tOS.iso`.

Lastly, run `make debug` to compile and run t/OS in a VM with GDB attached

## Who made this?

t/OS is written by [Andy Kolski](https://andyk.ca/)
