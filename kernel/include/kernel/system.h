#ifndef SYSTEM_H
#define SYSTEM_H



#include <libc/stdio.h>
#include <stdint.h>
#include <libc/assert.h>
#include <libc/string.h>
#include <stdbool.h>
#include <libc/ctype.h>

typedef void* type_t;

typedef int (*lessthan_predicate_t)(type_t,type_t);
typedef struct
{
  type_t *array;
  uint32_t size;
  uint32_t max_size;
  lessthan_predicate_t less_than;
} ordered_array_t;

struct cpu_regs
{
   uint32_t	ds;                                     /* Data segment selector */
   uint32_t	edi, esi, ebp, esp, ebx, edx, ecx, eax; /* Pushed by pusha. */
   uint32_t	int_no, err_code;                       /* Interrupt number and error code (if applicable) */
   uint32_t	eip, cs, eflags, useresp, ss;           /* Pushed by the processor automatically. */
};


typedef struct {
	uint32_t gs, fs, es, ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t int_no, err_code;
	uint32_t eip, cs, eflags, useresp, ss;
} __attribute__((packed)) registers_t;


#define KHEAP_MAX_ADDRESS 0xFFFF000

typedef struct page
{
    uint32_t present    : 1;   // Page present in memory
    uint32_t rw         : 1;   // Read-only if clear, readwrite if set
    uint32_t user       : 1;   // Supervisor level only if clear
    uint32_t accessed   : 1;   // Has the page been accessed since last refresh?
    uint32_t dirty      : 1;   // Has the page been written to since last refresh?
    uint32_t unused     : 7;   // Amalgamation of unused and reserved bits
    uint32_t frame      : 20;  // Frame address (shifted right 12 bits)
} page_t;

typedef struct page_table
{
    page_t pages[1024];
} page_table_t;

typedef struct page_directory
{
    /**
       Array of pointers to pagetables.
    **/
    page_table_t *tables[1024];
    /**
       Array of pointers to the pagetables above, but gives their *physical*
       location, for loading into the CR3 register.
    **/
    uint32_t tablesPhysical[1024];

    /**
       The physical address of tablesPhysical. This comes into play
       when we get our kernel heap allocated and the directory
       may be in a different location in virtual memory.
    **/
    uint32_t physicalAddr;
} page_directory_t;

// This structure defines a 'task' - a process.
typedef struct task
{
  uint32_t id; //Process ID

  //~ char name[32]; //Process name
  //~ uint32_t name; //Process name

  uint32_t esp, ebp; //Stack and base pointers.
  uint32_t eip; //Instruction pointer.

  uint32_t priority; //the priority of the process
  uint32_t burst_time; //the process burst time
  uint32_t averaged_priority; //A number that indicates average priority (priority and burst time)

  uint32_t time_to_run; //Time left on quanta
  uint32_t time_running; //Time spent running
  uint32_t ready_to_run; //is the process ready to run

  page_directory_t *page_directory; // Page directory.

  void (*thread)(); //thread entry point
  uint32_t thread_flags; //location of an array of thread arguments to go allong with that function

  uint32_t stack; //kernel stack location.
  uint32_t originalStack; //the original kernel stack location.

  struct task *next; //the next task in a linked list.
}task_t;


#include <kernel/va_list.h>
#include <kernel/multiboot.h>
#include <kernel/com.h>
#include <kernel/cpudetect.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/int32.h>
#include <kernel/irq.h>
#include <kernel/isr.h>
#include <kernel/keyboard.h>
#include <kernel/kheap.h>
#include <kernel/paging.h>
#include <kernel/shell.h>
#include <kernel/syscall.h>
#include <kernel/time.h>
#include <kernel/timer.h>
#include <kernel/tty.h>
#include <kernel/tar.h>
#include <kernel/vfs.h>
#include <kernel/initrd.h>
#include <kernel/task.h>
#include <kernel/schedule.h>

#define CLI() asm volatile("cli")
#define STI() asm volatile("sti")
#define HLT() asm volatile("hlt")

#define keep_running() while(true) { HLT(); }
#define PANIC(msg) panic(msg, __FILE__, __LINE__);
void panic(const char *msg, int line, char *file);
#define KASSERT(msg, cond) (                                       \
		(cond) ?                                           \
		(void)0 :                                          \
		panic("assertion failed: %s (%s)", (msg), (#cond)) \
)

#define ASSERT(b) ((b) ? (void)0 : panic(#b, __FILE__, __LINE__))
#define TRUE 1
#define FALSE 0
void reboot();
int rand( void ); // RAND_MAX assumed to be 32767
#endif
