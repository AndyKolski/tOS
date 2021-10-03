
# t/OS
t/OS is a simple hobby operating system that I started working on to learn more about the inner workings of modern x86 computers, x86 assembly, low-level code, and OS fundamentals. I have since found myself working on it more out of an interest in OS development than solely as an academic exercise.

Currently, t/OS is pretty bare-bones, but I'm slowly and steadily improving things, so it might be able to do something approaching useful eventually.

## Features & Things
Currently, t/OS has the following features:

- A graphical VESA terminal
- Full system date and time support
- Interrupt and exception handling
- Kernel memory management
- Keyboard and mouse support
- Paging
- PC Speaker support
- Serial support
- Very experimental ring-3 (userland) functionality
- Presumably, some other things

## Building & Running t/OS

### Dependencies
To build t/OS, you need to have the following dependencies installed:

- curl
- gcc
- grub-common
- make
- nasm
- qemu
- xorriso

On Debian/Ubuntu, you can install them all at once with this command:
```console
sudo apt install curl gcc grub-common make nasm qemu xorriso
```

I would be surprised if I haven't forgotten about at least a few dependencies, so if things aren't compiling correctly, this is probably a good place to start looking for solutions.

### Building the Toolchain
You will also need to build the toolchain (cross-compiler), and thankfully, there is a script to do it for you.

Just run the following script:
```console
Toolchain/build.sh
```

Depending on your computer, this part may take a while to finish, so expect to wait somewhere between a few minutes and a few hours if your computer is slower.

Optionally, once the script finishes compiling the toolchain, you can delete the "Build" and "Tarballs" folders from the toolchain folder to free up some space.

### Actually building t/OS 
Once you finish the previous steps, actually building t/OS is really simple.

To compile and run t/OS in a QEMU VM, just run `make run`.

To compile t/OS to a multiboot compatible file, run `make all`, and the result will be in `out/tOS.bin`.
To get a bootable ISO file, run `make iso`, and the ISO will be in `out/tOS.iso`.

Lastly, run `make debug` to compile and run t/OS in a VM with GDB attached.

## Who made this?
t/OS is written by [Andy Kolski](https://andyk.ca/)