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

void main_loop();

uint32_t test(char *test)
{
  if(!strcmp(test, "memory"))
  {
    uint32_t addr, size = 128;
    addr = kmalloc(size);
    kfree((void*)addr);

    //if we have not failed so far, print a sucessful message
    printf("\tMemory management test passed, allocated %d bytes at %h\n", size, addr);

    //sucess
    return 0;
  }/*else if(!strcmp(test, "tasking"))
  {
    //~ start_task(PRIO_HIGH, PROC_SHORT, tasking_test, "tasking", "tasking_test");

    s32int pid = fork(PRIO_LOW, PROC_SHORT, "test");

    k_printf("Testing multitasking...\n");

    if(!pid)
    {
      k_printf("\tMultitasking is operational with task PID: %h\n", getpid());
      exit();
    }

    //we have no error since if pid never equals 0, then something with assert, page fault etc.

    //sucess
    return 0;
  }
  */
  //something went wrong
  return 1;
}

#endif
