obj-m += bootkit.o payload.o startup.o hw.o debug.o helper.o
obj-y += startup.o 
CFLAGS += -I ../include -Werror
ASFLAGS += -I ../include

KERNELDIR ?= ../..
VMLINUX ?= $(KERNELDIR)/vmlinux
SYMBOLS ?= "./vmlinux.symbols"
OUTFILE ?= "linuxaddr.h"

all:
	./gen_addrs_hdr.sh $(VMLINUX) $(SYMBOLS) $(OUTFILE)
	CFLAGS=-I../include make -C $(KERNELDIR) M=$(PWD) modules
	CFLAGS=-I../include make -C $(KERNELDIR) M=$(PWD) V=1

clean:
	CFLAGS=-I../include make -C $(KERNELDIR) M=$(PWD) clean
	rm -f *.ko *.o
