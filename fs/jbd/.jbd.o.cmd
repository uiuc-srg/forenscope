cmd_fs/jbd/jbd.o := ld -m elf_i386 -m elf_i386  -r -o fs/jbd/jbd.o fs/jbd/transaction.o fs/jbd/commit.o fs/jbd/recovery.o fs/jbd/checkpoint.o fs/jbd/revoke.o fs/jbd/journal.o
