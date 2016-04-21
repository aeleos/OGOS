#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp hdd/boot/OGOS.kernel isodir/boot/OGOS.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "OGOS" {
	multiboot /boot/OGOS.kernel
}
EOF

grub-mkrescue -o OGOS.iso isodir
