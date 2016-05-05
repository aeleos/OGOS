

#ifndef TASK_H
#define TASK_H

#include <kernel/system.h>

#define KERNEL_STACK_SIZE 2048 // Use a 2kb kernel stack.

//process priorities
#define PRIO_DEAD 9900
#define PRIO_IDLE 0
#define PRIO_LOW 100
#define PRIO_MED 200
#define PRIO_HIGH 300

//process length times
#define PROC_VERY_LONG 10
#define PROC_LONG 30
#define PROC_MED 50
#define PROC_SHORT 70
#define PROC_VERY_SHORT 90


extern volatile task_t *ready_queue;

//The currently running task.
extern volatile task_t *current_task;

/*Initialises the tasking system*/
void initialise_tasking();

/*Called by the timer hook, this changes the running process to the next one in the list*/
void switch_task();

/*sets the input task as the current*/
void set_current_task(task_t *task_to_set);

/*fork_tasks the current process, spawning a new one with a different memory space*/
int fork_task(uint32_t priority, uint32_t burst_time, char *task_Name);

/*Causes the current process' stack to be forcibly moved to a new location*/
void move_stack(void *new_stack_start, uint32_t size);

/*Returns the pid of the current process*/
int getpid();

/*kills a task*/
int kill_task(uint32_t pid);

/*start a task*/
uint32_t start_task(uint32_t priority, uint32_t burst_time, void (*func)(), void *arg, char *task_Name);

/*a test for the multitasking*/
void tasking_test();

/*end the current task*/
void exit_task();

void __test__();

#endif
