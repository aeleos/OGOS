#include <stdio.h>

#include <kernel/timer.h>
#include <kernel/irq.h>
#include <kernel/com.h>
#include <kernel/sys.h>

// Globals are always initialized to 0
static uint32_t current_tick;

void timer_callback(registers_t* regs) {
	UNUSED(regs);

	current_tick++;
}

void init_timer() {
	irq_register_handler(IRQ0, &timer_callback);

	uint32_t divisor = TIMER_QUOTIENT / TIMER_FREQ;

	outportb(PIT_CMD, PIT_SET);
	outportb(PIT_0, divisor & 0xFF);
	outportb(PIT_0, (divisor >> 8) & 0xFF);

}



uint32_t timer_get_tick() {
	return current_tick;
}

double timer_get_time() {
	return current_tick*(1.0/TIMER_FREQ);
}