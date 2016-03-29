cmd_arch/i386/boot/setup := ld -m elf_i386 -m elf_i386  -Ttext 0x0 -s --oformat binary -e begtext arch/i386/boot/setup.o -o arch/i386/boot/setup 
