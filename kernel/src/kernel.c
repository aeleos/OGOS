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
#include <kernel/timer.h>
#include <kernel/pmm.h>
#include <kernel/paging.h>

extern uint32_t KERNEL_BEGIN_PHYS;
extern uint32_t KERNEL_END_PHYS;
extern uint32_t KERNEL_SIZE;

void kernel_main(multiboot* boot, uint32_t magic) {
	init_term();
	printf("Loading kernel...\n");
	printf("Kernel loaded at 0x%X, ending at 0x%X (%dKiB)\n", &KERNEL_BEGIN_PHYS, &KERNEL_END_PHYS,
		((uint32_t) &KERNEL_SIZE) >> 10);

	assert(magic == MULTIBOOT_EAX_MAGIC);
	assert(boot->flags & MULTIBOOT_FLAG_MMAP);

	init_gdt();
	init_idt();
	init_irq();
	init_timer();
	init_pmm(boot);
	init_paging();

	term_change_bg_color(COLOR_DARK_GREY);
	term_setcolor(COLOR_GREEN, COLOR_DARK_GREY);
	printf("\n\n");
	printf("8\"\"\"88 8\"\"\"\"8 8\"\"\"88 8\"\"\"\"8 \n");
	printf("8    8 8    \" 8    8 8      \n");
	printf("8    8 8e     8    8 8eeeee \n");
	printf("8    8 88  ee 8    8     88 \n");
	printf("8    8 88   8 8    8 e   88 \n");
	printf("8eeee8 88eee8 8eeee8 8eee88 \n");

	term_setcolor(COLOR_LIGHT_GREY, COLOR_DARK_GREY);
	printf("Welcome to \x1B[36mOGOS\x1B");
	term_setcolor(COLOR_LIGHT_GREY, COLOR_DARK_GREY);

	printf(" -1.0 !\n\n");


	uint32_t time = 0;
	while (1) {
		uint32_t ntime = (uint32_t) timer_get_time();
		if (ntime > time) {
			time = ntime;
			printf("\x1B[s\x1B[24;75H"); // save & move cursor
			printf("\x1B[K");            // Clear line
			printf("%ds", time);
			printf("\x1B[u");            // Restore cursor
		}
	}

}
