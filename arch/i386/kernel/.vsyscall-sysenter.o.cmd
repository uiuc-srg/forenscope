cmd_arch/i386/kernel/vsyscall-sysenter.o := gcc -m32 -Wp,-MD,arch/i386/kernel/.vsyscall-sysenter.o.d  -nostdinc -isystem /usr/lib/gcc/i486-linux-gnu/4.2.3/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -D__ASSEMBLY__   -Iinclude/asm-i386/mach-default    -c -o arch/i386/kernel/vsyscall-sysenter.o arch/i386/kernel/vsyscall-sysenter.S

deps_arch/i386/kernel/vsyscall-sysenter.o := \
  arch/i386/kernel/vsyscall-sysenter.S \
  arch/i386/kernel/vsyscall-sigreturn.S \
  include/asm/unistd.h \
  include/asm/asm-offsets.h \

arch/i386/kernel/vsyscall-sysenter.o: $(deps_arch/i386/kernel/vsyscall-sysenter.o)

$(deps_arch/i386/kernel/vsyscall-sysenter.o):