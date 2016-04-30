#!/bin/bash

sudo /sbin/losetup /dev/loop1 floppy.img
sudo mount /dev/loop1 /mnt
sudo cp sysroot/boot/OGOS.kernel /mnt/kernel
sudo cp ./initrd/initrd.tar /mnt/initrd
sudo umount /dev/loop1
sudo /sbin/losetup -d /dev/loop1
