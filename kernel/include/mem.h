#ifndef MEM_H_
#define MEM_H_

#include <stdint.h>
#include <stddef.h>

uint8_t farpeekb(uint16_t, void*);
uint16_t farpeekw(uint16_t, void*);
uint32_t farpeekl(uint16_t, void*);

void farpokeb(uint16_t, void*, uint8_t);
void farpokew(uint16_t, void*, uint16_t);
void farpokel(uint16_t, void*, uint32_t);

int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);

#endif
