cmd_fs/partitions/built-in.o :=  ld -m elf_i386 -m elf_i386  -r -o fs/partitions/built-in.o fs/partitions/check.o fs/partitions/ldm.o fs/partitions/msdos.o fs/partitions/efi.o
