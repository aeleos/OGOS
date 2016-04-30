#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/OGOS.kernel isodir/boot/OGOS.kernel
cp initrd/initrd.tar isodir/boot/initrd.tar
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "OGOS" {
	multiboot /boot/OGOS.kernel
	module /boot/initrd.tar
}
EOF

grub-mkrescue -o OGOS.iso isodir
