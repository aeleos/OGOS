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
#include <kernel/cpudetect.h>
#include <kernel/keyboard.h>
#include <kernel/syscall.h>

extern uint32_t KERNEL_BEGIN_PHYS;
extern uint32_t KERNEL_END_PHYS;
extern uint32_t KERNEL_SIZE;




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
	init_timer();
	init_pmm(boot);
	init_paging();
	init_syscall();
	init_keyboard();
	keyboard_wait_press();
	init_term();
	term_change_bg_color(COLOR_DARK_GREY);
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
	detect_cpu();
	uint32_t time = 0;
	while (1) {
		uint32_t ntime = (uint32_t) timer_get_time();
		if (ntime > time) {
			time = ntime;
			printf("\x1B[s\x1B[0;75H"); // save & move cursor
			printf("\x1B[1K");            // Clear line
			printf("\x1B[3m\x1B[37m\x1B[40m%ds", time);
			printf("\x1B[4m\x1B[u");            // Restore cursor
			move_cursor();
		}
	}


}
