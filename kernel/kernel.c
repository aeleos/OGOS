#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <tty.h>
#include <descriptor_tables.h>
#include <timer.h>


int kernel_main(){
  init_descriptor_tables();
  terminal_initialize();
  printf("Hello, you absolute piece of shit\nSuck poop JackS\n");
  printf("Number test: %d\n", 1000);
  printf("Binary test: %x\n", 1000);
  if (are_interrupts_enabled()){
    printf("Interrupts enabled");
  } else {
    printf("Interrupts disabled");
  }

  init_timer(50);

  //asm volatile ("int $0x3");
  //asm volatile ("int $0x4");
  //return 0;
}
