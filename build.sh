#!/bin/bash
if [ ! -d out ]; then
	mkdir out
fi

yasm ./boot/Stage1_BootSec.asm -I. -fbin -o ./out/temp.img
yasm ./boot/stage2/stage2_loader.asm -I. -fbin -o ./out/KRNLDR.LIG

yasm ./kern/krnasm.asm -I. -felf32 -o ./out/krnlasm.obj

i686-elf-gcc -c ./kern/ckern.c -o ./out/ckern.obj -O2 -std=gnu99 -ffreestanding -Wall -Wextra -pedantic
i686-elf-gcc -c ./kern/term.c -o ./out/term.obj -O2 -std=gnu99 -ffreestanding -Wall -Wextra -pedantic
i686-elf-gcc -c ./kern/mem.c -o ./out/mem.obj -O2 -std=gnu99 -ffreestanding -Wall -Wextra -pedantic
i686-elf-gcc -c ./kern/command.c -o ./out/command.obj -O2 -std=gnu99 -ffreestanding -Wall -Wextra -pedantic
i686-elf-gcc -c ./kern/string.c -o ./out/string.obj -O2 -std=gnu99 -ffreestanding -Wall -Wextra -pedantic
i686-elf-gcc -c ./kern/disk.c -o ./out/disk.obj -O2 -std=gnu99 -ffreestanding -Wall -Wextra -pedantic
i686-elf-gcc -c ./kern/elf.c -o ./out/elf.obj -O2 -std=gnu99 -ffreestanding -Wall -Wextra -pedantic
i686-elf-gcc -c ./kern/syscall.c -o ./out/syscall.obj -O2 -std=gnu99 -ffreestanding -Wall -Wextra -pedantic
i686-elf-gcc -c ./kern/tss.c -o ./out/tss.obj -O2 -std=gnu99 -ffreestanding -Wall -Wextra -pedantic
i686-elf-gcc -T ./linker/kern.ld -o ./out/KRNL.LIG -O2 -ffreestanding -nostdlib ./out/syscall.obj ./out/tss.obj ./out/elf.obj ./out/disk.obj ./out/krnlasm.obj ./out/string.obj ./out/term.obj ./out/mem.obj ./out/command.obj ./out/ckern.obj -lgcc

yasm ./stage4/st4asm.asm -I. -felf32 -o ./out/st4asm.obj
i686-elf-gcc -c ./stage4/st4.c -o ./out/st4.obj -O2 -std=gnu99 -ffreestanding -Wall -Wextra -pedantic
i686-elf-gcc -c ./stage4/term.c -o ./out/st4term.obj -O2 -std=gnu99 -ffreestanding -Wall -Wextra -pedantic
i686-elf-gcc -T ./linker/st4.ld -o ./out/ST4.LIG -O2 -ffreestanding -nostdlib ./out/st4asm.obj ./out/st4.obj ./out/st4term.obj -lgcc

i686-elf-gcc -c ./prog/prog.c -o ./out/tprog.obj -O2 -std=gnu99 -ffreestanding -Wall -Wextra -pedantic
i686-elf-gcc -T ./prog/link.ld -o ./fs/TPROG.ELF -O2 -ffreestanding -nostdlib ./out/tprog.obj -lgcc

mcopy -i ./out/temp.img ./out/KRNLDR.LIG ::/
mcopy -i ./out/temp.img ./out/ST4.LIG ::/
mcopy -i ./out/temp.img ./out/KRNL.LIG ::/
mcopy -i ./out/temp.img ./fs/TEST.TXT ::/
mcopy -i ./out/temp.img ./fs/TPROG.ELF ::/

cp ./out/temp.img ./build.img
