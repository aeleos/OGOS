#include <kernel/timer.h>

int tick;
void* std_in;
volatile int in_size;
uint8_t inbuffer[STDIO_SIZE];


void timer_callback(registers_t* regs) {
	CLI();
	tick++;
	if (in_size >= STDIO_SIZE) {
			in_size = 0;
	}
	STI();
}

void init_timer() {
	irq_register_handler(IRQ0, &timer_callback);

	uint32_t divisor = TIMER_QUOTIENT / TIMER_FREQ;

	outportb(PIT_CMD, PIT_SET);
	outportb(PIT_0, divisor & 0xFF);
	outportb(PIT_0, (divisor >> 8) & 0xFF);

}



uint32_t timer_get_tick() {
	return tick;
}
