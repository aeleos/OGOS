#ifndef _STDIO_H
#define _STDIO_H 1


#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <tty.h>

void print(const char*, size_t);
int printf(const char* __restrict, ...);
int putchar(int);
int puts(const char*);
char* itoa(int, char*, int);
void outb(uint16_t, uint8_t);
void outw(uint16_t, uint16_t);
void outl(uint16_t, uint32_t);
uint8_t inb(uint16_t);
uint16_t inw(uint16_t);
uint32_t inl(uint16_t);
void io_wait(void);

#ifdef __cplusplus
}
#endif

#endif
