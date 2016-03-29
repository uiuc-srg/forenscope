CC=`which gcc`
BASE=0x90909090
LDFLAGS=-Ttext=$(BASE) -static
all: 
	@echo Making libs
	@cd lib && $(MAKE)
	@echo Making core
	@cd core && $(MAKE)
	@echo Linking
	@diet $(CC) $(LDFLAGS) -s -nostdlib lib/*.o core/*.o -o mood-nt 
	@echo Done!
	
	
clean:
	cd lib && make clean
	cd core && make clean
	rm mood-nt 
