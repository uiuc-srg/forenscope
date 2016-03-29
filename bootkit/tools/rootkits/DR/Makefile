obj-m := DR.o
#KERN_DIR=/lib/modules/`uname -r`/build
KERN_DIR=../../../..

all:
	$(MAKE) -C $(KERN_DIR) M=`pwd` modules
clean:
	$(MAKE) -C $(KERN_DIR) M=`pwd` clean
	$(RM) Module.markers modules.order
