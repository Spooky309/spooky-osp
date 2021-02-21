compile: yes
running: yes

## what the devil is this?
This is a dumb little toy kernel I made a while back for fun, it uses paging a heap allocator and has a terminal with commands and stuff. It can't run programs yet and the kernel doesn't load things from the disk (although the bootloader does)
Right now, it only works with ATA, and the disk that's used has to be a master disk, otherwise the stage 4 bootloader won't work. Sorry, I don't make the rules. 

This may be the most useless operating system in existence, as it cannot yet load binaries, or schedule.

# DEPS:
* yasm (edit build.sh if you want to use nasm instead)
* i686-elf-gcc/binutils toolchain
* `mtools` installed

# building and running:
1. run build.sh from the src directory and it will create build.img in the src directory
2. load it into hda in bochs or qemu-system-i386

# notes:
1. you can use cygwin to build on windows
2. if you use arch linux, build the cross-compiler yourself, the aur package is busted as of 2018/12/23
3. you don't *need* mcopy, for instance, on windows you can mount the image and put KRNLDR.LIG and KERN.LIG on it yourself (if you do so, make sure you remove the mcopy line from build.sh)
