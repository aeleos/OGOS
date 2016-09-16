#ifndef KERNEL_H
#define KERNEL_H


#include <kernel/tty.h>
#include <kernel/multiboot.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/irq.h>
#include <kernel/kheap.h>
#include <kernel/paging.h>
#include <kernel/cpudetect.h>
#include <kernel/keyboard.h>
#include <kernel/syscall.h>
#include <kernel/time.h>
#include <kernel/shell.h>
#include <kernel/com.h>
#include <kernel/timer.h>
#include <kernel/initrd.h>
#include <kernel/task.h>
#include <kernel/schedule.h>


void main_loop();

uint32_t test(char *test)
{
  if(!strcmp(test, "memory"))
  {
    uint32_t addr, size = 128;
    addr = kmalloc(size);
    kfree((void*)addr);

    //if we have not failed so far, print a sucessful message
    printf("\tMemory management test passed, allocated %d bytes at 0x%X\n", size, addr);

    //sucess
    return 0;
  }else if(!strcmp(test, "tasking"))
  {
    //~ start_task(PRIO_HIGH, PROC_SHORT, tasking_test, "tasking", "tasking_test");


    int pid = fork_task(PRIO_LOW, PROC_SHORT, "test");

    printf("Testing multitasking...\n");

    if(!pid)
    {
      printf("\tMultitasking is operational with task PID: %h\n", getpid());
      exit_task();
    }

    //we have no error since if pid never equals 0, then something with assert, page fault etc.

    //sucess
    return 0;
  }

  //something went wrong
  return 1;
}


void
vfs_print_content(void)
{
	// list the contents of /
	int i = 0;
	struct dirent *node = 0;
	while ((node = vfs_readdir(vfs_root, i))) {
  		printf("Found file %s", node->name);
  		struct vfs_node *fsnode = vfs_finddir(vfs_root, node->name);

  		if (VFS_ISDIR(fsnode))
    			printf("\n\t(directory)\n");
  		else {
    			printf("\n\t contents:\n");
    			char buf[1024 + 1];
    			uint32_t sz = vfs_read(fsnode, 0, 1024, buf);
			buf[sz] = '\0';
      			printf("%s\n", buf);
  		}
  		i++;
	}
}
#endif
