rm -rf iso

mkdir -p iso/boot/grub
#cp -f /usr/lib/grub/i386-pc/stage2_eltorito iso/boot/grub
cp -f tools/stage2_eltorito iso/boot/grub
cp -f config/menu.lst.cdrom iso/boot/grub/menu.lst
cp -f images/bzImage iso/boot/bootjacker-bzImage
cp -f kernel iso/boot/bootjacker-kernel
strip iso/boot/bootjacker-kernel
cp -f images/System.map iso/boot/bootjacker-System.map
cp -f syscall.bin iso/
cp -rf vdisk iso/
cp -rf vdisktools iso/
cp -f vdisktools/rcS iso/
mkdir -p iso/lib
ln -sf /cdrom/vdisktools/root iso/root
cp -f /lib/* iso/lib
objcopy -j .text.head -j .text --gap-fill 0 -O binary vmlinux iso/boot/bootjacker-fixtext

mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o grub.iso iso
