yasm ./boot/Stage1_BootSec.asm -I. -fbin -o ./out/temp.img
yasm ./boot/stage2/stage2_loader.asm -I. -fbin -o ./out/KRNLDR.LIG

yasm ./kern/krnasm.asm -I. -felf32 -o ./out/krnlasm.obj
i686-elf-gcc -c ./kern/ckern.c -o ./out/ckern.obj -O2 -std=gnu99 -ffreestanding -Wall -Wextra
i686-elf-gcc -c ./kern/term.c -o ./out/term.obj -O2 -std=gnu99 -ffreestanding -Wall -Wextra -msoft-float -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -mno-80387
i686-elf-gcc -c ./kern/mem.c -o ./out/mem.obj -O2 -std=gnu99 -ffreestanding -Wall -Wextra -msoft-float -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -mno-80387
i686-elf-gcc -c ./kern/command.c -o ./out/command.obj -O2 -std=gnu99 -ffreestanding -Wall -Wextra -msoft-float -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -mno-80387
i686-elf-gcc -c ./kern/string.c -o ./out/string.obj -O2 -std=gnu99 -ffreestanding -Wall -Wextra -msoft-float -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -mno-80387
i686-elf-gcc -T ./linker/link.ld -o ./out/KRNL.LIG -O2 -ffreestanding -nostdlib ./out/krnlasm.obj ./out/string.obj ./out/term.obj ./out/mem.obj ./out/command.obj ./out/ckern.obj -lgcc

mcopy -i ./out/temp.img ./out/KRNLDR.LIG ::/
mcopy -i ./out/temp.img ./out/KRNL.LIG ::/
cp ./out/temp.img ./build.img
