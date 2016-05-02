#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <kernel/kernel.h>

char user[20];
char machine[30];
extern uint32_t placement_address;
uint32_t initial_esp;
uint32_t initrd_location;
uint32_t initrd_end;


void kernel_main(multiboot* boot, uint32_t initial_stack) {
	tty_menu_clear();
	tty_init();
	tty_set_cursor(0,0);

	//dump_multiboot_infos(boot);
	//printf("[Kernel] Loading kernel...\n");
	//printf("[Kernel] Loaded at 0x%X, ending at 0x%X (%dKiB)\n", &KERNEL_BEGIN_PHYS, &KERNEL_END_PHYS,
	//	((uint32_t) &KERNEL_SIZE) >> 10);

	//assert(magic == MULTIBOOT_EAX_MAGIC);
	assert(boot->flags & MULTIBOOT_FLAG_MMAP);
	assert(boot->mods_count > 0);
	detect_cpu();
	init_gdt();
	init_idt();
	init_irq();
	//init_pmm(boot);
//	init_paging();
//	init_syscall();
	init_keyboard();
	init_stdin();
	initial_esp = initial_stack;

	initrd_location = *((uint32_t*)boot->mods_addr);
  initrd_end = *(uint32_t*)(boot->mods_addr + 4);
	placement_address = initrd_end; // XXX: hacky here

	uint32_t memorySize = ((boot->mem_lower + boot->mem_upper) * 1024); //Bytes

	printf("Size of memory: %d KB, %d MB\n", memorySize / 1024, memorySize / (1024 * 1024));

	initialise_paging(memorySize);

	vfs_root = init_initrd((void *)initrd_location);

	vfs_print_content();

	init_timer();
	test("memory");
	printf("Press any key to continue...");
	getch();
	tty_clear();
	printf("\x1B[1m\n\n");
	tty_setcolor(COLOR_GREEN, COLOR_DARK_GREY);
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
