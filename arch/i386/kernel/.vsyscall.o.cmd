cmd_arch/i386/kernel/vsyscall.o := gcc -m32 -Wp,-MD,arch/i386/kernel/.vsyscall.o.d  -nostdinc -isystem /usr/lib/gcc/i486-linux-gnu/4.2.3/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -D__ASSEMBLY__   -Iinclude/asm-i386/mach-default    -c -o arch/i386/kernel/vsyscall.o arch/i386/kernel/vsyscall.S

deps_arch/i386/kernel/vsyscall.o := \
  arch/i386/kernel/vsyscall.S \
  include/linux/init.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/hotplug.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/acpi/hotplug/memory.h) \
  include/linux/compiler.h \
    $(wildcard include/config/enable/must/check.h) \

arch/i386/kernel/vsyscall.o: $(deps_arch/i386/kernel/vsyscall.o)

$(deps_arch/i386/kernel/vsyscall.o):
