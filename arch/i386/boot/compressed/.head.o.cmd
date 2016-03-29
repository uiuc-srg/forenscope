cmd_arch/i386/boot/compressed/head.o := gcc -m32 -Wp,-MD,arch/i386/boot/compressed/.head.o.d  -nostdinc -isystem /usr/lib/gcc/i486-linux-gnu/4.2.3/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -D__ASSEMBLY__   -Iinclude/asm-i386/mach-default -traditional   -c -o arch/i386/boot/compressed/head.o arch/i386/boot/compressed/head.S

deps_arch/i386/boot/compressed/head.o := \
  arch/i386/boot/compressed/head.S \
    $(wildcard include/config/relocatable.h) \
    $(wildcard include/config/physical/align.h) \
  include/linux/linkage.h \
  include/asm/linkage.h \
    $(wildcard include/config/x86/alignment/16.h) \
  include/asm/segment.h \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/paravirt.h) \
  include/asm/page.h \
    $(wildcard include/config/x86/use/3dnow.h) \
    $(wildcard include/config/x86/pae.h) \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/highmem4g.h) \
    $(wildcard include/config/highmem64g.h) \
    $(wildcard include/config/page/offset.h) \
    $(wildcard include/config/flatmem.h) \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/out/of/line/pfn/to/page.h) \
  include/asm-generic/page.h \
  include/asm/boot.h \
    $(wildcard include/config/physical/start.h) \

arch/i386/boot/compressed/head.o: $(deps_arch/i386/boot/compressed/head.o)

$(deps_arch/i386/boot/compressed/head.o):
