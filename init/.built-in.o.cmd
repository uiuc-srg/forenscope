cmd_init/built-in.o :=  ld -m elf_i386 -m elf_i386  -r -o init/built-in.o init/main.o init/version.o init/mounts.o init/initramfs.o init/calibrate.o
