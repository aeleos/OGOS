#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/ogos.kernel isodir/boot/ogos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "ogos" {
	multiboot /boot/ogos.kernel
}
EOF
grub-mkrescue -o ogos.iso isodir
