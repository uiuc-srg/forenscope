cmd_arch/i386/boot/setup.o := gcc -m32 -Wp,-MD,arch/i386/boot/.setup.o.d  -nostdinc -isystem /usr/lib/gcc/i486-linux-gnu/4.2.3/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -D__ASSEMBLY__   -Iinclude/asm-i386/mach-default -DSVGA_MODE=NORMAL_VGA  -D__BIG_KERNEL__   -c -o arch/i386/boot/setup.o arch/i386/boot/setup.S

deps_arch/i386/boot/setup.o := \
  arch/i386/boot/setup.S \
    $(wildcard include/config/physical/align.h) \
    $(wildcard include/config/relocatable.h) \
    $(wildcard include/config/x86/voyager.h) \
    $(wildcard include/config/x86/speedstep/smi.h) \
    $(wildcard include/config/apm.h) \
    $(wildcard include/config/x86/elan.h) \
  include/asm/segment.h \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/paravirt.h) \
  include/linux/utsrelease.h \
  include/linux/compile.h \
  include/asm/boot.h \
    $(wildcard include/config/physical/start.h) \
  include/asm/e820.h \
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
  include/asm/setup.h \
  include/linux/pfn.h \
  arch/i386/boot/../kernel/verify_cpu.S \
    $(wildcard include/config/x86/minimum/cpu/model.h) \
  include/asm/cpufeature.h \
  include/asm/required-features.h \
    $(wildcard include/config/x86/minimum/cpu.h) \
    $(wildcard include/config/x86/cmov.h) \
    $(wildcard include/config/x86/cmpxchg64.h) \
  include/asm/msr.h \
  include/asm/msr-index.h \
  arch/i386/boot/edd.S \
    $(wildcard include/config/edd.h) \
  include/linux/edd.h \
  arch/i386/boot/video.S \
    $(wildcard include/config/video/svga.h) \
    $(wildcard include/config/video/vesa.h) \
    $(wildcard include/config/video/compact.h) \
    $(wildcard include/config/video/retain.h) \
    $(wildcard include/config/video/local.h) \
    $(wildcard include/config/video/400/hack.h) \
    $(wildcard include/config/video/gfx/hack.h) \
    $(wildcard include/config/video/select.h) \
    $(wildcard include/config/firmware/edid.h) \

arch/i386/boot/setup.o: $(deps_arch/i386/boot/setup.o)

$(deps_arch/i386/boot/setup.o):
