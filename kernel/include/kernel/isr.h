#ifndef ISR_H
#define ISR_H

#include <kernel/system.h>


typedef void (*handler_t)(registers_t*);

void isr_register_handler(uint32_t num, handler_t handler);

#endif
