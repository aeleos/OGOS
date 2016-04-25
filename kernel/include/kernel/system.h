#ifndef SYSTEM_H
#define SYSTEM_H


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
	uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t int_no, err_code;
	uint32_t eip, cs, eflags, useresp, ss;
} __attribute__((packed)) registers_t;

#include <kernel/multiboot.h>
#include <kernel/com.h>
#include <kernel/cpudetect.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/int32.h>
#include <kernel/irq.h>
#include <kernel/isr.h>
#include <kernel/keyboard.h>
#include <kernel/liballoc.h>
#include <kernel/paging.h>
#include <kernel/pmm.h>
#include <kernel/shell.h>
#include <kernel/syscall.h>
#include <kernel/time.h>
#include <kernel/timer.h>
#include <kernel/tty.h>

#define CLI() asm volatile("cli")
#define STI() asm volatile("sti")
#define HLT() asm volatile("hlt")

#define keep_running() while(true) { HLT(); }

void panic(const char *msg, int line, char *file);
void reboot();

#endif
