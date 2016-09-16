#ifndef STDIO_H
#define STDIO_H

#include <libc/types.h>
#include <kernel/va_list.h>

#ifdef __cplusplus
extern "C" {
#endif
#define STDIO_SIZE 4096
int printf(const char* __restrict, ...);
int putchar(int);
int puts(const char*);
int getchar(void);
int getch(void);
char *gets(char *str);



#ifdef __cplusplus
}
#endif

#endif
