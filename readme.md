
# t/OS

t/OS is a simple hobby operating system that I started working on to learn more about the inner workings of modern x86 computers, x86 assembly, low-level code, and OS fundamentals. I have since found myself working on it more out of an interest in OS development than solely as an academic exercise.

Currently, t/OS is pretty bare-bones, but I'm slowly and semi-steadily improving things, so it might be able to do something approaching useful eventually.

## Features & Things

Currently, t/OS has the following features:

- Fully 64-bit kernel
- A graphical VESA terminal
- Full system date and time support
- Interrupt and exception handling
- Kernel memory management
- Keyboard and mouse support
- Paging and a higher-half kernel
- PC Speaker support
- Serial support
- Presumably, some other things

## Building & Running t/OS

A Linux-based environment is required to build t/OS. It should also be possible to build t/OS on other Unix-like OSs, or on Windows using WSL or Cygwin, but I haven't tested it. If you try it, and especially if it works, let me know how it went!

### Dependencies

To build t/OS, you need to have the following dependencies installed:

- A C & C++ compiler
- Make
- GMP
- MPC
- MPFR
- Texinfo
- cURL
- NASM
- GRUB tools
- xorriso
- QEMU x86_64

On Debian/Ubuntu and their derivatives, you can install them all at once with this command:

```console
sudo apt-get install build-essential curl grub-common libgmp-dev libmpc-dev libmpfr-dev nasm qemu-system-gui qemu-system-x86 qemu-utils texinfo xorriso 
```

I think this should be all of the dependencies, but if things aren't compiling correctly, this is probably a good place to start looking for solutions.

### Building the Toolchain

You will also need to build the toolchain (cross-compiler); thankfully, there is a script to do it for you.

Note that the script downloads the toolchain's source code, so you need an internet connection to use it. The download is about 200 MiB and will only happen the first time you run the script. Subsequent runs will use the existing source code if you haven't deleted it to save space.

The build process will temporarily use about 2 GiB of disk space, but the final toolchain will be about 200 MiB.

Just run the following script:

```console
Toolchain/build.sh
```

Depending on your computer, this part may take a while to finish, so expect to wait somewhere between a few minutes and a few hours if your computer is slower.

Optionally, once the script finishes compiling the toolchain, you can delete the "Build" and "Tarballs" folders from the Toolchain folder to free up most of the disk space used during the build process.

### Actually building t/OS

Once you finish the previous steps, actually building t/OS is really simple.

To compile and run t/OS in a QEMU VM, just run `make run`.

To compile t/OS to a multiboot 2 compatible file, run `make all`, and the result will be in `out/tOS.bin`.
To get a bootable ISO file, run `make iso`, and the ISO will be in `out/tOS.iso`.

Lastly, run `make debug` to compile and run t/OS with GDB attached.

## Who made this?

t/OS is written by [Andy Kolski](https://andyk.ca/)
