cmd_arch/i386/lib/putuser.o := gcc -m32 -Wp,-MD,arch/i386/lib/.putuser.o.d  -nostdinc -isystem /usr/lib/gcc/i486-linux-gnu/4.2.3/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -D__ASSEMBLY__   -Iinclude/asm-i386/mach-default    -c -o arch/i386/lib/putuser.o arch/i386/lib/putuser.S

deps_arch/i386/lib/putuser.o := \
  arch/i386/lib/putuser.S \
  include/linux/linkage.h \
  include/asm/linkage.h \
    $(wildcard include/config/x86/alignment/16.h) \
  include/asm/dwarf2.h \
    $(wildcard include/config/unwind/info.h) \
    $(wildcard include/config/as/cfi/signal/frame.h) \
  include/asm/thread_info.h \
    $(wildcard include/config/4kstacks.h) \
    $(wildcard include/config/debug/stack/usage.h) \
  include/linux/compiler.h \
    $(wildcard include/config/enable/must/check.h) \
  include/asm/page.h \
    $(wildcard include/config/x86/use/3dnow.h) \
    $(wildcard include/config/x86/pae.h) \
    $(wildcard include/config/paravirt.h) \
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
  include/asm/asm-offsets.h \

arch/i386/lib/putuser.o: $(deps_arch/i386/lib/putuser.o)

$(deps_arch/i386/lib/putuser.o):
