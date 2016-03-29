CC 	= gcc-3.4
AS 	= gcc-3.4
BOCHS	= tools/bochs
DEFINES	= -DHAVE_CONFIG_H
STARTADDR=0x40000
INCLUDES= -I. -I. -I.. -Iinclude
#WARNERROR ?= -Werror
WARNINGS= -Wall -Wmissing-prototypes -Wunused -Wshadow -Wpointer-arith -Wundef $(WARNERROR)
OPTIMIZATIONS = -falign-jumps=1 -falign-loops=1 -falign-functions=1
OSFLAGS = -fno-builtin -nostdinc -O -imacros config.h -g -nostdlib 
OSLFLAGS= -Wl,-N -Wl,-Ttext -Wl,$(STARTADDR) -Wl,-e,_start 
DEBUG	= -g
LAST_ADDR=0x`nm vmlinux | sort | tail -n 1 | cut -d " " -f 1`
LAST_ADDR_DEF=-DLAST_ADDR=$(LAST_ADDR)
CFLAGS 	= $(INCLUDES) $(WARNINGS) $(OPTIMIZATIONS) $(OSFLAGS) $(DEBUG) $(LAST_ADDR_DEF)
ASFLAGS = $(CFLAGS)
LDFLAGS = $(INCLUDES) $(OSFLAGS) $(OSLFLAGS) $(DEBUG) --startgroup src/boot.o --endgroup
HEADERS=grub_addr
TARGETS=kernel.o src/linuxstub.o src/linuxstubc.o src/helper.o src/grub/lspci.o src/grub/pci.o src/zlib/deflate.o src/zlib/zutil.o src/zlib/adler32.o src/zlib/crc32.o src/zlib/trees.o src/cmostime.o src/cmostime_asm.o src/quicklz.o src/md5.o
KMOD_TARGET=module/payload.o module/startup.o module/hw.o module/debug.o module/helper.o module/elevator_hook.o

all: $(HEADERS) modules kernel busy 

modules: src/boot.o
	echo "#define LAST_ADDR $(LAST_ADDR)" > module/linuxmisc.h
	cd module && make 

kernel: $(TARGETS) $(KMOD_TARGET) 
#	gcc -o kernel $(TARGETS) $(KMOD_TARGET) $(LDFLAGS) 
 
grub_addr:
	./module/gen_addrs_hdr.sh tools/pre_stage2.exec src/grub.symbols src/grub_addr.h
	./scripts/dump_syscall_table.sh

clean:
	rm -f $(TARGETS) kernel src/boot.o
	cd module && make clean
	rm -f logbuf
	rm -f busy
	rm -f syscall.gdb syscall.bin
	rm -f grub.iso

../ramdisk/rdtsc:
	make -C rdtsc
	cp rdtsc/rdtsc ../ramdisk/rdtsc

busy:
	gcc -o busy src/busy.c -static
	cp busy ../ramdisk

../ramdisk/dev/.mkconsole.stamp: ../ramdisk/dev/mkconsole
	-(cd ../ramdisk/dev/ && ./mkconsole)
	touch $@

../.config: config/bootkit_config
	@cp -v $< $@

qemu: ../.config ../ramdisk/dev/.mkconsole.stamp busy ../ramdisk/rdtsc
	cd .. && make -j20
	make start

start: all
	./start

bochs: all
	./scripts/gen_iso_img.sh
	$(BOCHS) -q -f config/bochs.config	

.PHONY: all modules grub_addr

install: all
	sudo cp -f kernel /boot/bootjacker-kernel
	sudo strip /boot/bootjacker-kernel
	sudo cp -f images/bzImage /boot/bootjacker-bzImage

installreb: install
	sudo reboot

tags:
	ctags -R --exclude=tools
