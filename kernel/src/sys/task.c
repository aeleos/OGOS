

#include <kernel/task.h>

//The currently running task.
volatile task_t *current_task;

//The start of the task linked list.
volatile task_t *ready_queue;

//Some externs are needed to access members in paging.c...
extern page_directory_t *kernel_directory;
extern page_directory_t *current_directory;
extern void alloc_frame(page_t*,int,int);
extern uint32_t initial_esp;
extern uint32_t read_eip();

//The next available process ID.
uint32_t next_pid = 1;
uint32_t nTasks = 0;

void initialise_tasking()
{
  asm volatile("cli");

  move_stack((void*)0x10000000, 0x2000);

  uint32_t *stack;
  task_t *task = (task_t*)kmalloc(sizeof(task_t));
  current_task = ready_queue = task;
  task->stack = kmalloc(0x1000) + 0x1000;	// Allocate 4 kilobytes of space
  task->esp = task->stack;
  task->originalStack = task->stack - 0x1000; //set the originalStack to its starting location

  // Clone the address space.
  task->page_directory = current_directory;

  task->priority = PRIO_LOW;
  task->time_to_run = 10;
  task->time_running = 0;
  task->ready_to_run = TRUE;

  /*for the most part, this process will not be running anything so the average
   * burst time will be near 0 */
  task->burst_time = 0;
  task->averaged_priority = 0;

  nTasks++;

  stack = (uint32_t*)task->stack;

// processor data (iret)
  *--stack = 0x202;	// EFLAGS
  *--stack = 0x08;	// CS
//~ *--stack = (uint32_t)func; // EIP
  *--stack = 0;	// EIP
  task->eip = 0;

// pusha
  *--stack = 0;	// EDI
  *--stack = 0;	// ESI
  *--stack = 0;	// EBP
  task->ebp = 0;

  *--stack = 0;	// NULL
  *--stack = 0;	// EBX
  *--stack = 0;	// EDX
  *--stack = 0;	// ECX
  *--stack = 0;	// EAX

// data segments
  *--stack = 0x10;	// DS
  *--stack = 0x10;	// ES
  *--stack = 0x10;	// FS
  *--stack = 0x10;	// GS

  task->id = next_pid++;
//~ task->id = 0;
  task->stack = (uint32_t)stack;
  task->thread = 0;
  task->thread_flags = 0;

  //TODO FIX THIS NAME STUFF
  //~ uint32_t name_len = strlen("init");
  //~ task->name = kmalloc(name_len + 1); //+1 for the \000
  //~ strcpy((u8int*)task->name, "init");
  //~ *((u8int*)task->name + name_len) = 0;

  //~ strcpy(task->name, "init");

  task->next = 0;

  /*set the scheduling algorithm to prioritized SJF
   * see the schedule.h for a table of contents for
   * what number represents what algorithm to use */
  set_scheduling_algorithm(1);

  asm volatile("sti");

}

void move_stack(void *new_stack_start, uint32_t size)
{
  uint32_t i;
  // Allocate some space for the new stack.
  for(i = (uint32_t)new_stack_start;
      i >= ((uint32_t)new_stack_start - size);
      i -= 0x1000)
  {
    // General-purpose stack is in user-mode.
    alloc_frame( get_page(i, 1, current_directory), 0 /* User mode */, 1 /* Is writable */ );
  }

  // Flush the TLB by reading and writing the page directory address again.
  uint32_t pd_addr;
  asm volatile("mov %%cr3, %0" : "=r" (pd_addr));
  asm volatile("mov %0, %%cr3" : : "r" (pd_addr));

  // Old ESP and EBP, read from registers.
  uint32_t old_stack_pointer; asm volatile("mov %%esp, %0" : "=r" (old_stack_pointer));
  uint32_t old_base_pointer; asm volatile("mov %%ebp, %0" : "=r" (old_base_pointer));

  // Offset to add to old stack addresses to get a new stack address.
  uint32_t offset = (uint32_t)new_stack_start - initial_esp;

  // New ESP and EBP.
  uint32_t new_stack_pointer = old_stack_pointer + offset;
  uint32_t new_base_pointer = old_base_pointer + offset;

  // Copy the stack.
  memcpy((void*)new_stack_pointer, (void*)old_stack_pointer, initial_esp-old_stack_pointer);

  // Backtrace through the original stack, copying new values into
  // the new stack.
  for(i = (uint32_t)new_stack_start; i > (uint32_t)new_stack_start - size; i -= 4)
  {
    uint32_t tmp = * (uint32_t*)i;
    // If the value of tmp is inside the range of the old stack, assume it is a base pointer
    // and remap it. This will unfortunately remap ANY value in this range, whether they are
    // base pointers or not.
    if (( old_stack_pointer < tmp) && (tmp < initial_esp))
    {
      tmp = tmp + offset;
      uint32_t *tmp2 = (uint32_t*)i;
      *tmp2 = tmp;
    }
  }

  // Change stacks.
  asm volatile("mov %0, %%esp" : : "r" (new_stack_pointer));
  asm volatile("mov %0, %%ebp" : : "r" (new_base_pointer));
}

void set_current_task(task_t *task_to_set)
{
  // If we haven't initialised tasking yet, just return.
  if (current_task == 0)
    return;

  asm volatile("cli");

  //Checks if there is there such a task
  if(!task_to_set)
  {
    asm volatile("sti");

    return;
  }

  // Read esp, ebp now for saving later on.
  uint32_t esp, ebp, eip;
  asm volatile("mov %%esp, %0" : "=r"(esp));
  asm volatile("mov %%ebp, %0" : "=r"(ebp));

  // Read the instruction pointer. We do some cunning logic here:
  // One of two things could have happened when this function exit_tasks -
  // (a) We called the function and it returned the EIP as requested.
  // (b) We have just switched tasks, and because the saved EIP is essentially
  // the instruction after read_eip(), it will seem as if read_eip has just
  // returned.
  // In the second case we need to return immediately. To detect it we put a dummy
  // value in EAX further down at the end of this function. As C returns values in EAX,
  // it will look like the return value is this dummy value! (0x12345).
  eip = read_eip();

  // Have we just switched tasks?
  if (eip == 0x12345)
    return;

  // No, we didn't switch tasks. Let's save some register values and switch.
  current_task->eip = eip;
  current_task->esp = esp;
  current_task->ebp = ebp;

  // Get the next task to run.
  current_task = task_to_set;

  eip = current_task->eip;
  esp = current_task->esp;
  ebp = current_task->ebp;

  // Make sure the memory manager knows we've changed page directory.
  if(current_task->page_directory)
  {
    current_directory = current_task->page_directory;
  }

  //~ printf("\nCurrent Process is: %d\n", printfid());

  // Change our kernel stack over.
  //set_kernel_stack(current_task->kernel_stack+KERNEL_STACK_SIZE);

  // Here we:
  // * Stop interrupts so we don't get interrupted.
  // * Temporarily put the new EIP location in ECX.
  // * Load the stack and base pointers from the new task struct.
  // * Change page directory to the physical address (physicalAddr) of the new directory.
  // * Put a dummy value (0x12345) in EAX so that above we can recognise that we've just
  // switched task.
  // * Restart interrupts. The STI instruction has a delay - it doesn't take effect until after
  // the next instruction.
  // * Jump to the location in ECX (remember we put the new EIP in there).
  asm volatile(" \
cli; \
mov %0, %%ecx; \
mov %1, %%esp; \
mov %2, %%ebp; \
mov %3, %%cr3; \
mov $0x12345, %%eax; \
sti; \
jmp *%%ecx "
               : : "r"(eip), "r"(esp), "r"(ebp), "r"(current_directory->physicalAddr));

  asm volatile("sti");

}

void switch_task()
{
  // If we haven't initialised tasking yet, just return.
  if(!current_task)
    return;

  asm volatile("cli");

  // Read esp, ebp now for saving later on.
  uint32_t esp, ebp, eip;
  asm volatile("mov %%esp, %0" : "=r"(esp));
  asm volatile("mov %%ebp, %0" : "=r"(ebp));

  // Read the instruction pointer. We do some cunning logic here:
  // One of two things could have happened when this function exit_tasks -
  // (a) We called the function and it returned the EIP as requested.
  // (b) We have just switched tasks, and because the saved EIP is essentially
  // the instruction after read_eip(), it will seem as if read_eip has just
  // returned.
  // In the second case we need to return immediately. To detect it we put a dummy
  // value in EAX further down at the end of this function. As C returns values in EAX,
  // it will look like the return value is this dummy value! (0x12345).
  eip = read_eip();

  // Have we just switched tasks?
  if (eip == 0x12345)
    return;

  // No, we didn't switch tasks. Let's save some register values and switch.
  current_task->eip = eip;
  current_task->esp = esp;
  current_task->ebp = ebp;

  // Get the next task to run.
  current_task = current_task->next;

  // If we fell off the end of the linked list start again at the beginning.
  if(!current_task)
  {
    current_task = ready_queue;
  }

  eip = current_task->eip;
  esp = current_task->esp;
  ebp = current_task->ebp;

  // Make sure the memory manager knows we've changed page directory.
  if(current_task->page_directory)
  {
    current_directory = current_task->page_directory;
  }

  // Here we:
  // * Stop interrupts so we don't get interrupted.
  // * Temporarily put the new EIP location in ECX.
  // * Load the stack and base pointers from the new task struct.
  // * Change page directory to the physical address (physicalAddr) of the new directory.
  // * Put a dummy value (0x12345) in EAX so that above we can recognise that we've just
  // switched task.
  // * Restart interrupts. The STI instruction has a delay - it doesn't take effect until after
  // the next instruction.
  // * Jump to the location in ECX (remember we put the new EIP in there).
  asm volatile(" \
cli; \
mov %0, %%ecx; \
mov %1, %%esp; \
mov %2, %%ebp; \
mov %3, %%cr3; \
mov $0x12345, %%eax; \
sti; \
jmp *%%ecx "
               : : "r"(eip), "r"(esp), "r"(ebp), "r"(current_directory->physicalAddr));

  asm volatile("sti");

}

int fork_task(uint32_t priority, uint32_t burst_time, char *task_Name)
{

  asm volatile("cli");

  // Take a pointer to this process' task struct for later reference.
  static task_t *parent_task;
  parent_task = (task_t*)current_task;

  uint32_t id = next_pid++;

  uint32_t *stack;
  task_t *task = (task_t*)kmalloc(sizeof(task_t));
  task->stack = kmalloc(0x1000) + 0x1000;	// Allocate 4 kilobytes of space
  task->esp = task->stack;
  task->originalStack = task->stack - 0x1000; //set the originalStack to its starting location

  // Clone the address space.
  page_directory_t *directory = clone_directory(current_directory);
  task->page_directory = directory;

  task->priority = priority;
  task->time_to_run = 0;
  task->time_running = 0;
  task->ready_to_run = TRUE;

  task->burst_time = burst_time;
  task->averaged_priority = priority + burst_time;

  nTasks++;

  stack = (uint32_t*)task->stack;

  // processor data (iret)
  *--stack = 0x202;	// EFLAGS
  *--stack = 0x08;	// CS
  *--stack = 0;	// EIP
  task->eip = 0;

  // pusha
  *--stack = 0;	// EDI
  *--stack = 0;	// ESI
  *--stack = 0;	// EBP
  task->ebp = 0;
  *--stack = 0;	// NULL
  *--stack = 0;	// EBX
  *--stack = 0;	// EDX
  *--stack = 0;	// ECX
  *--stack = 0;	// EAX

  // data segments
  *--stack = 0x10;	// DS
  *--stack = 0x10;	// ES
  *--stack = 0x10;	// FS
  *--stack = 0x10;	// GS

  task->id = id;
  task->stack = (uint32_t)stack;
  task->thread = 0;
  task->thread_flags = 0;

  //TODO this name stuff is still a problem
  //~ uint32_t name_len = strlen(task_Name);
  //~ task->name = kmalloc(name_len + 1); //+1 for the \000
  //~ strcpy((u8int*)task->name, task_Name);
  //~ *((u8int*)task->name + name_len) = 0;

  task->next = 0;

  preempt_task(task);

  uint32_t eip = read_eip();

  // We could be the parent or the child here - check.
  if(current_task == parent_task) //we are the child
  {
    // We are the parent, so set up the esp/ebp/eip for our child.
    uint32_t esp; asm volatile("mov %%esp, %0" : "=r"(esp));
    uint32_t ebp; asm volatile("mov %%ebp, %0" : "=r"(ebp));
    task->esp = esp;
    task->ebp = ebp;
    task->eip = eip;
    // All finished: Reenable interrupts.
    asm volatile("sti");

    // And by convention return the PID of the child.
    return task->id;
  }else{ //we are the parent
    //~ printf("\nPID %d\n", getpid());

    // We are the child - by convention return 0.
    return 0;
  }

  asm volatile("sti");

  return id;
}

int getpid()
{
  return current_task->id;
}


void exit_task()
{
  asm volatile("cli");
  //Just incase the removal doesn't work 100%
  //we make sure we are using as little time as possible
  current_task->priority = PRIO_DEAD;
  current_task->time_to_run = 0;
  current_task->ready_to_run = 0;

  //Find previous task
  task_t *task_prev = 0;
  task_t *task_r = (task_t*)ready_queue;
  for(;task_r->next != 0; task_r = task_r->next)
  {
    if(task_r->next == current_task)
    {
      //We got the previous task
      task_prev = task_r;
      break; //Don't bother with the rest of the list
    }
  }

  //We didn't find the task and it is not the ready_queue
  if(!task_prev && current_task != ready_queue)
    return;

  //if our current task is the ready_queue then set the starting task as the next task after current_task
  if(current_task == ready_queue)
  {
    ready_queue = current_task->next;
  }else{
    task_prev->next = current_task->next;
  }

  nTasks--;

  //Free the memory
  kfree((void*)current_task);
  kfree((void*)current_task->originalStack);
  kfree((void*)current_task->page_directory);

  asm volatile("sti"); //Restart Interrupts before switching - stop CPU lockup
  switch_task(); //Don't waste any time
}

uint32_t start_task(uint32_t priority, uint32_t burst_time, void (*func)(), void *arg, char *task_Name)
{

  asm volatile("cli");

  //Take a pointer to this process' task struct for later reference.
  //task_t *parent_task = (task_t*)current_task;

  uint32_t id = next_pid++;

  uint32_t *stack;
  task_t *task = (task_t*)kmalloc(sizeof(task_t));
  task->stack = kmalloc(0x1000) + 0x1000;	// Allocate 4 kilobytes of space
  task->esp = task->stack;
  task->originalStack = task->stack - 0x1000; //set the originalStack to its starting location

  // Clone the address space.
  //page_directory_t *directory = clone_directory(current_directory);
  //task->page_directory = directory;

  //this is a task (process) not a fork_task, we do not need a cloned address space
  task->page_directory = 0;

  task->priority = priority;
  task->time_to_run = 0;
  task->time_running = 0;
  task->ready_to_run = TRUE;

  task->burst_time = burst_time;
  task->averaged_priority = priority + burst_time;

  nTasks++;

  stack = (uint32_t*)task->stack;

  // processor data (iret)
  *--stack = 0x202;	// EFLAGS
  *--stack = 0x08;	// CS
  *--stack = (uint32_t)func;	// EIP
  task->eip = (uint32_t)func;

  // pusha
  *--stack = 0;	// EDI
  *--stack = 0;	// ESI
  *--stack = 0;	// EBP
  task->ebp = 0;
  *--stack = 0;	// NULL
  *--stack = 0;	// EBX
  *--stack = 0;	// EDX
  *--stack = 0;	// ECX
  *--stack = 0;	// EAX

  // data segments
  *--stack = 0x10;	// DS
  *--stack = 0x10;	// ES
  *--stack = 0x10;	// FS
  *--stack = 0x10;	// GS

  task->id = id;
  task->stack = (uint32_t)stack;
  task->thread = func;
  task->thread_flags = (uint32_t)arg;
  //~ strcpy(task->name, task_Name);

  task->next = 0;

  //preempt the task
  preempt_task(task);

  asm volatile("sti");

  return id;
}

int kill_task(uint32_t pid)
{
  task_t *task_prev = 0;
  task_t *task_r = (task_t*)ready_queue;
  for(;task_r->next != 0; task_r = task_r->next)
  {
    //~ if(task_r->id == pid)
    if(task_r->next->id == pid)
    {
      //We got the previous task
      task_prev = task_r;
    }
  }

  //we did not find our task
  if(!task_prev)
    return 1;

  task_r = task_prev->next;
  task_prev->next = task_r->next;

  nTasks--;

  kfree((void*)task_r);
  return 0;
}

void switch_to_user_mode()
{
  // Set up our kernel stack.
  //set_kernel_stack(current_task->kernel_stack+KERNEL_STACK_SIZE);

  // Set up a stack structure for switching to user mode.
  asm volatile(" \
cli; \
mov $0x23, %ax; \
mov %ax, %ds; \
mov %ax, %es; \
mov %ax, %fs; \
mov %ax, %gs; \
\
\
mov %esp, %eax; \
pushl $0x23; \
pushl %esp; \
pushf; \
pushl $0x1B; \
push $1f; \
iret; \
1: \
");

}

void tasking_test()
{
  char *arguements;

  //if there is an arg tied to the current_task, set it to arguements
  asm volatile("cli");
  if(current_task->thread_flags)
  {

    arguements = (char*)current_task->thread_flags;
  }
  asm volatile("sti");

  /*we send the a string of "tasking" allong with the current task
   * if everything is intact, we can be sure multitasking works */
  if(!strcmp(arguements, "tasking"))
    printf("\tMultitasking is operational with task PID: %h\n", getpid());
  else
    printf("\t%crMultitasking has failed%cw\n");

  //delete the process from the multitasking queue
  exit_task();
}

void __test__()
{

  uint32_t pid = fork_task(PRIO_LOW, PROC_MED, "wopw");

  if(!pid)
  {
    printf("child\n");
    exit_task();
  }
  else
  {
    printf("parent\n");

  }
}
