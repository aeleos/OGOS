#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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
#define TIMER_IRQ IRQ0
#define TIMER_FREQ 1000 // in Hz
#define TIMER_QUOTIENT 1193180

#define PIT_0 0x40
#define PIT_1 0x41
#define PIT_2 0x42
#define PIT_CMD 0x43
#define PIT_SET 0x36


int tick;
extern volatile int in_size;

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




extern uint32_t KERNEL_BEGIN_PHYS;
extern uint32_t KERNEL_END_PHYS;
extern uint32_t KERNEL_SIZE;
void* stdin;
volatile int in_size;
uint8_t inbuffer[STDIO_SIZE];
void main_loop();
char user[20];
char machine[30];


void kernel_main(multiboot* boot, uint32_t magic) {
	term_menu_clear();
	init_term();
	term_set_cursor(0,0);

	dump_multiboot_infos(boot);
	printf("[Kernel] Loading kernel...\n");
	printf("[Kernel] Loaded at 0x%X, ending at 0x%X (%dKiB)\n", &KERNEL_BEGIN_PHYS, &KERNEL_END_PHYS,
		((uint32_t) &KERNEL_SIZE) >> 10);

	assert(magic == MULTIBOOT_EAX_MAGIC);
	assert(boot->flags & MULTIBOOT_FLAG_MMAP);
	init_gdt();
	init_idt();
	init_irq();

	init_pmm(boot);
	init_paging();
	init_syscall();
	init_keyboard();
	init_stdin();

	init_timer();

	printf("Press any key to continue...");
	getch();
	term_clear();
	printf("\x1B[1m\n\n");
	term_setcolor(COLOR_GREEN, COLOR_DARK_GREY);
	printf("8\"\"\"88 8\"\"\"\"8 8\"\"\"88 8\"\"\"\"8 \n");
	printf("8    8 8    \" 8    8 8      \n");
	printf("8    8 8e     8    8 8eeeee \n");
	printf("8    8 88  ee 8    8     88 \n");
	printf("8    8 88   8 8    8 e   88 \n");
	printf("8eeee8 88eee8 8eeee8 8eee88 \n\x1B[2m");

	printf("Welcome to \x1B[1m\x1B[36mOGOS\x1B[2m");

	printf(" -1.0 !\n\n");
	time_install();
	//detect_cpu();
	//print_entry_info(0,9);
	printf("Username: ");
	gets(user);
	printf("Machine: ");
	gets(machine);
	//printf("%s", user);
	main_loop();

}
void main_loop(){
	char cmd[1024];
	while (true) {
		memset(cmd, 0, 1023);
		printf("%s@%s:$ ", user, machine);
		gets(cmd);
		if (cmd[0] != 0) {
			if(shell(cmd)) {
					printf("Command '%s' not found.\n", cmd);
			}
		}
}

}
