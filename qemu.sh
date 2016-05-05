#!/bin/sh
set -e
. ./iso.sh

qemu-system-i386 -cdrom OGOS.iso -monitor stdio -s -d int,cpu_reset -D irq.log -m 4086M
