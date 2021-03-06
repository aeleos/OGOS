#ifndef TIMER_H
#define TIMER_H

#include <kernel/system.h>


#define TIMER_IRQ IRQ0
#define TIMER_FREQ 1000 // in Hz
#define TIMER_QUOTIENT 1193180

#define PIT_0 0x40
#define PIT_1 0x41
#define PIT_2 0x42
#define PIT_CMD 0x43
#define PIT_SET 0x36


void timer_callback(registers_t* regs);
void init_timer(void);
uint32_t timer_get_tick(void);

#endif
