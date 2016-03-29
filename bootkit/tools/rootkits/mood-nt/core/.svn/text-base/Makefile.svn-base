CC=`which diet`
CCFLAGS=gcc -s -I /usr/include/diet -I ../include/ -w -fno-builtin -nostdlib -O2 -c
all: charger.c
	$(CC) $(CCFLAGS) ./charger.c
	./buildobject

clean:
	rm -f ./*.o
	rm -f ./*.s
