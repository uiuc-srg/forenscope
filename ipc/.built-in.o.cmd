cmd_ipc/built-in.o :=  ld -m elf_i386 -m elf_i386  -r -o ipc/built-in.o ipc/util.o ipc/msgutil.o ipc/msg.o ipc/sem.o ipc/shm.o ipc/ipc_sysctl.o
