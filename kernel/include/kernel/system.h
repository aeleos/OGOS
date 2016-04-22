#ifndef SYSTEM_H
#define SYSTEM_H

#include <kernel/types.h>

#define CLI() asm volatile("cli")
#define STI() asm volatile("sti")
#define HLT() asm volatile("hlt")

#define keep_running() while(true) { HLT(); }

void panic(const char *msg, int line, char *file);
void reboot();

#endif
