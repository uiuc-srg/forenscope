cmd_/home/ubuntu/forenscope/bootkit/module/startup.o := gcc -m32 -Wp,-MD,/home/ubuntu/forenscope/bootkit/module/.startup.o.d  -nostdinc -isystem /usr/lib/gcc/i486-linux-gnu/4.2.3/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -D__ASSEMBLY__   -Iinclude/asm-i386/mach-default    -c -o /home/ubuntu/forenscope/bootkit/module/startup.o /home/ubuntu/forenscope/bootkit/module/startup.S

deps_/home/ubuntu/forenscope/bootkit/module/startup.o := \
  /home/ubuntu/forenscope/bootkit/module/startup.S \
  include/linux/linkage.h \
  include/asm/linkage.h \
    $(wildcard include/config/x86/alignment/16.h) \
  include/asm/desc.h \
    $(wildcard include/config/paravirt.h) \
  include/asm/ldt.h \
  include/asm/segment.h \
    $(wildcard include/config/smp.h) \
  include/asm/cache.h \
    $(wildcard include/config/x86/l1/cache/shift.h) \
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
  /home/ubuntu/forenscope/bootkit/module/linuxaddr.h \
  /home/ubuntu/forenscope/bootkit/module/state.h \

/home/ubuntu/forenscope/bootkit/module/startup.o: $(deps_/home/ubuntu/forenscope/bootkit/module/startup.o)

$(deps_/home/ubuntu/forenscope/bootkit/module/startup.o):
