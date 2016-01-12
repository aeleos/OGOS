#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <tty.h>
#include <assert.h>
#include <multiboot.h>
#include <gdt.h>
#include <idt.h>
#include <irq.h>

#include <timer.h>
#include <vga.h>

/*
8"""88 8""""8 8"""88 8""""8
8    8 8    " 8    8 8
8    8 8e     8    8 8eeeee
8    8 88  ee 8    8     88
8    8 88   8 8    8 e   88
8eeee8 88eee8 8eeee8 8eee88

*/

extern uint32_t KERNEL_BEGIN_PHYS;
extern uint32_t KERNEL_END_PHYS;
extern uint32_t KERNEL_SIZE;



int kernel_main(multiboot* boot, uint32_t magic){
  terminal_initialize();
  printf("Welcome to \x1B[36mSnowflakeOS\x1B[37m -1.0 !\n\n");
  printf("Kernel loaded at 0x%X, ending at 0x%X (%dKiB)\n", &KERNEL_BEGIN_PHYS, &KERNEL_END_PHYS,
  ((uint32_t) &KERNEL_SIZE) >> 10);

  assert(magic == MULTIBOOT_EAX_MAGIC);
  assert(boot->flags & MULTIBOOT_FLAG_MMAP);

  init_gdt();
  init_idt();
  init_irq();

  terminal_setcolor(make_color(COLOR_GREEN, COLOR_DARK_GREY));
  printf("8\"\"\"88 8\"\"\"\"8 8\"\"\"88 8\"\"\"\"8 \n");
  printf("8    8 8    \" 8    8 8      \n");
  printf("8    8 8e     8    8 8eeeee \n");
  printf("8    8 88  ee 8    8     88 \n");
  printf("8    8 88   8 8    8 e   88 \n");
  printf("8eeee8 88eee8 8eeee8 8eee88 \n");

  terminal_setcolor(make_color(COLOR_LIGHT_GREY, COLOR_DARK_GREY));
  printf("\nHello, you absolute piece of shit\nFuck you Aram\n");
  printf("Number test: %d\n", 1000);
  printf("Binary test: %x\n", 1000);
  //init_timer(50);

  //asm volatile ("int $0x3");
  //asm volatile ("int $0x11");
  //return 0;
  init_timer(50);
  //asm volatile ("sti");
  for(;;) {
   asm("hlt");
  }
}
