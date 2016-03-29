cmd_arch/i386/crypto/twofish-i586-asm.o := gcc -m32 -Wp,-MD,arch/i386/crypto/.twofish-i586-asm.o.d  -nostdinc -isystem /usr/lib/gcc/i486-linux-gnu/4.2.3/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h -D__ASSEMBLY__   -Iinclude/asm-i386/mach-default    -c -o arch/i386/crypto/twofish-i586-asm.o arch/i386/crypto/twofish-i586-asm.S

deps_arch/i386/crypto/twofish-i586-asm.o := \
  arch/i386/crypto/twofish-i586-asm.S \
  include/asm/asm-offsets.h \

arch/i386/crypto/twofish-i586-asm.o: $(deps_arch/i386/crypto/twofish-i586-asm.o)

$(deps_arch/i386/crypto/twofish-i586-asm.o):
