cmd_arch/i386/lib/checksum.o := gcc -m32 -Wp,-MD,arch/i386/lib/.checksum.o.d  -nostdinc -isystem /usr/lib/gcc/i486-linux-gnu/4.2.3/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -D__ASSEMBLY__   -Iinclude/asm-i386/mach-default    -c -o arch/i386/lib/checksum.o arch/i386/lib/checksum.S

deps_arch/i386/lib/checksum.o := \
  arch/i386/lib/checksum.S \
    $(wildcard include/config/x86/use/ppro/checksum.h) \
  include/linux/linkage.h \
  include/asm/linkage.h \
    $(wildcard include/config/x86/alignment/16.h) \
  include/asm/dwarf2.h \
    $(wildcard include/config/unwind/info.h) \
    $(wildcard include/config/as/cfi/signal/frame.h) \
  include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \

arch/i386/lib/checksum.o: $(deps_arch/i386/lib/checksum.o)

$(deps_arch/i386/lib/checksum.o):
