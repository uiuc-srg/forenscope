CC=`which diet`
CCFLAGS=gcc -w -I /usr/include/diet/ -fno-builtin -I../include/ -nostdlib -c -O2
all: rk.c
	@$(CC) $(CCFLAGS) ./rk.c
clean:
	@rm -f ./*.o

