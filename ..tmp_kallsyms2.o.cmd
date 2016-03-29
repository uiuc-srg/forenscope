cmd_.tmp_kallsyms2.o := gcc -m32 -Wp,-MD,./..tmp_kallsyms2.o.d -D__ASSEMBLY__   -Iinclude/asm-i386/mach-default   -nostdinc -isystem /usr/lib/gcc/i486-linux-gnu/4.2.3/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h    -c -o .tmp_kallsyms2.o .tmp_kallsyms2.S

deps_.tmp_kallsyms2.o := \
  .tmp_kallsyms2.S \
  include/asm/types.h \
    $(wildcard include/config/highmem64g.h) \

.tmp_kallsyms2.o: $(deps_.tmp_kallsyms2.o)

$(deps_.tmp_kallsyms2.o):
