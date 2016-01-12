#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>
extern size_t terminal_row;
extern size_t terminal_column;
void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_clrscreen(void);
void terminal_scroll(void);
void terminal_setcolor(uint8_t);
void move_cursor(void);
#endif
