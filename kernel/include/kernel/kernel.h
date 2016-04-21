#ifndef KERNEL_H
#define KERNEL_H

#include <kernel/tty.h>
#include <kernel/multiboot.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/irq.h>
#include <kernel/pmm.h>
#include <kernel/paging.h>
#include <kernel/cpudetect.h>
#include <kernel/keyboard.h>
#include <kernel/syscall.h>
#include <kernel/time.h>
#include <kernel/shell.h>
#include <kernel/com.h>
#include <kernel/timer.h>
#include <kernel/video.h>

void main_loop();

//#define VID

#endif
