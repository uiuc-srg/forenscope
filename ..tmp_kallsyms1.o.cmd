cmd_.tmp_kallsyms1.o := gcc -m32 -Wp,-MD,./..tmp_kallsyms1.o.d -D__ASSEMBLY__   -Iinclude/asm-i386/mach-default   -nostdinc -isystem /usr/lib/gcc/i486-linux-gnu/4.2.3/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h    -c -o .tmp_kallsyms1.o .tmp_kallsyms1.S

deps_.tmp_kallsyms1.o := \
  .tmp_kallsyms1.S \
  include/asm/types.h \
    $(wildcard include/config/highmem64g.h) \

.tmp_kallsyms1.o: $(deps_.tmp_kallsyms1.o)

$(deps_.tmp_kallsyms1.o):
