obj-m += northbridge.o
obj-y += 
CFLAGS += 
ASFLAGS += 

KERNELDIR=/lib/modules/`uname -r`/build

all:
	make -C $(KERNELDIR) M=$(PWD) modules
	make -C $(KERNELDIR) M=$(PWD) V=1

clean:
	make -C $(KERNELDIR) M=$(PWD) clean
	rm -f *.ko *.o
