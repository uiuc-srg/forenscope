cmd_arch/i386/boot/vmlinux.bin := objcopy -O binary -R .note -R .comment -S  arch/i386/boot/compressed/vmlinux arch/i386/boot/vmlinux.bin
