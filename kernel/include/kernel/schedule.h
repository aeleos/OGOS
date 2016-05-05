

#ifndef SCHEDULE
#define SCHEDULE

#include <kernel/system.h>

/*a pointer to a function that rearranges the tasks in some order*/
void (*rearrange_schedule)(void);

/*our scheduling algorithm*/
void schedule();

/*****************************
 * algorthms table contents  *
 * 0-prioritized round robin *
 * 1-prioritized SJF         *
/*****************************/
void set_scheduling_algorithm(uint32_t algorithm_number);

///scheduling algorithms///

/*arranges the tasks in the order of a prioritzed SJF (shorstest job first)*/
void sjf_arrange();

/*puts a task in its propper location in the list when initilalizing the task*/
void preempt_task(task_t *task_to_preempt);

#endif
