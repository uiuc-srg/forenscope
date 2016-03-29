cmd_arch/i386/boot/bootsect := ld -m elf_i386 -m elf_i386  -Ttext 0x0 -s --oformat binary arch/i386/boot/bootsect.o -o arch/i386/boot/bootsect 
