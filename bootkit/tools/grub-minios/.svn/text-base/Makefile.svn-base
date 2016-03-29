CC 	= gcc-3.4
AS 	= gcc-3.4
DEFINES	= -DHAVE_CONFIG_H
INCLUDES= -I. -I. -I.. 
WARNINGS= -Wall -Wmissing-prototypes -Wunused -Wshadow -Wpointer-arith -Wundef
OPTIMIZATIONS = -falign-jumps=1 -falign-loops=1 -falign-functions=1
OSFLAGS = -fno-builtin -nostdinc -O -imacros config.h -g -nostdlib -Wl,-N -Wl,-Ttext -Wl,100000
DEBUG	= -g
CFLAGS 	= $(INCLUDES) $(WARNINGS) $(OPTIMIZATIONS) $(OSFLAGS) $(DEBUG)
ASFLAGS = $(CFLAGS)
LDFLAGS = $(CFLAGS)

TARGETS= kernel.o boot.o

all: kernel

kernel: $(TARGETS)

clean:
	rm -f $(TARGETS) kernel

.PHONY: all
