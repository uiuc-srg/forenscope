cmd_arch/i386/kernel/vmlinux.lds := gcc -m32 -E -Wp,-MD,arch/i386/kernel/.vmlinux.lds.d  -nostdinc -isystem /usr/lib/gcc/i486-linux-gnu/4.2.3/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h  -P -C -Ui386 -D__ASSEMBLY__ -o arch/i386/kernel/vmlinux.lds arch/i386/kernel/vmlinux.lds.S

deps_arch/i386/kernel/vmlinux.lds := \
  arch/i386/kernel/vmlinux.lds.S \
    $(wildcard include/config/blk/dev/initrd.h) \
  include/asm-generic/vmlinux.lds.h \
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
  include/asm/cache.h \
    $(wildcard include/config/x86/l1/cache/shift.h) \
  include/asm/boot.h \
    $(wildcard include/config/physical/start.h) \
    $(wildcard include/config/physical/align.h) \

arch/i386/kernel/vmlinux.lds: $(deps_arch/i386/kernel/vmlinux.lds)

$(deps_arch/i386/kernel/vmlinux.lds):
