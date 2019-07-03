compile: yes
running: yes

## what the devil is this?
This is a dumb little toy kernel I made a while back for fun, it uses paging (poorly) and has the worst memory allocator of all time. But, it does have a terminal driver which works nicely (but lacks a backbuffer) and a custom bootloader that may or may not work on your machine. Although I used many resources for the bootloader, I made a point to avoid using tutorials for a lot of C stuff and kinda freestyle it.

This may be the most useless operating system in existence, as it cannot yet load binaries, or even read from a disk (ATA gives me a serious headache.)

# DEPS:
* yasm (edit build.sh if you want to use nasm instead)
* i686-elf-gcc/binutils toolchain
* `mtools` installed

# building and running:
1. run build.sh from the src directory and it will create build.img in the src directory
2. load it into fda in bochs or qemu-system-i386 (also tested using a usb drive on real hardware, obligatory "works on my machine")

# notes:
1. you can use cygwin to build on windows
2. if you use arch linux, build the cross-compiler yourself, the aur package is busted as of 2018/12/23
3. you don't *need* mcopy, for instance, on windows you can mount the image and put KRNLDR.LIG and KERN.LIG on it yourself (if you do so, make sure you remove the mcopy line from build.sh)
