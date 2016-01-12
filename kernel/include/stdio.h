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

//void print(const char*, size_t);
int printf(const char* __restrict, ...);
int putchar(int);
int puts(const char*);
char* itoa(int, char*, int);
unsigned char inportb(uint16_t port);
void outportb(uint16_t port, uint8_t data);
uint16_t inports(uint16_t port);
void outports(uint16_t port, uint16_t data);
uint32_t inportl(uint16_t port);
void outportl(uint16_t port, uint32_t data);
void inportsm(uint16_t port, unsigned char * data, uint32_t size);
void outportsm(uint16_t port, unsigned char * data, uint32_t size);
//void io_wait(void);

#ifdef __cplusplus
}
#endif

#endif
