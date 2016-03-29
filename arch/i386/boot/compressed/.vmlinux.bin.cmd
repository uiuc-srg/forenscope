cmd_arch/i386/boot/compressed/vmlinux.bin := objcopy -O binary -R .note -R .comment -S  vmlinux arch/i386/boot/compressed/vmlinux.bin
