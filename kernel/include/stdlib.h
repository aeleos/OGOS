#ifndef _STDLIB_H
#define _STDLIB_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>


__attribute__((__noreturn__))
void abort(void);
size_t strlen(const char*);

bool are_interrupts_enabled();
void lidt(void* base, uint16_t size);

void cpuid(int code, uint32_t* a, uint32_t* d);
uint64_t rdtsc();
unsigned long read_cr0(void);
void invlpg(void* m);
void wrmsr(uint32_t msr_id, uint64_t msr_value);
uint64_t rdmsr(uint32_t msr_id);

#ifdef __cplusplus
}
#endif

#endif
