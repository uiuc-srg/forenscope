cmd_arch/i386/kernel/head.o := gcc -m32 -Wp,-MD,arch/i386/kernel/.head.o.d  -nostdinc -isystem /usr/lib/gcc/i486-linux-gnu/4.2.3/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -D__ASSEMBLY__   -Iinclude/asm-i386/mach-default    -c -o arch/i386/kernel/head.o arch/i386/kernel/head.S

deps_arch/i386/kernel/head.o := \
  arch/i386/kernel/head.S \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/early/printk.h) \
  include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  include/linux/linkage.h \
  include/asm/linkage.h \
    $(wildcard include/config/x86/alignment/16.h) \
  include/asm/segment.h \
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
  include/asm/pgtable.h \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/highpte.h) \
  include/asm-generic/pgtable.h \
  include/asm/desc.h \
  include/asm/ldt.h \
  include/asm/cache.h \
    $(wildcard include/config/x86/l1/cache/shift.h) \
  include/asm/thread_info.h \
    $(wildcard include/config/4kstacks.h) \
    $(wildcard include/config/debug/stack/usage.h) \
  include/linux/compiler.h \
    $(wildcard include/config/enable/must/check.h) \
  include/asm/asm-offsets.h \
  include/asm/setup.h \
  include/linux/pfn.h \

arch/i386/kernel/head.o: $(deps_arch/i386/kernel/head.o)

$(deps_arch/i386/kernel/head.o):
