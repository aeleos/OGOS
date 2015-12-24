#include <stdlib.h>

size_t strlen(const char* string)
{
	size_t result = 0;
	while ( string[result] )
		result++;
	return result;
}

void cpuid(int code, uint32_t* a, uint32_t* d){
  asm volatile ( "cpuid" : "=a"(*a), "=d"(*d) : "0"(code) : "ebx", "ecx");
}

uint64_t rdtsc(){
  uint64_t ret;
  asm volatile ( "rdtsc" : "=A"(ret) );
  return ret;
}

unsigned long read_cr0(void){
  unsigned long val;
  asm volatile ( "mov %%cr0, %0" : "=r"(val) );
  return val;
}

void invlpg(void* m){
  asm volatile ( "invlpg (%0)" : : "b"(m) : "memory");
}

void wrmsr(uint32_t msr_id, uint64_t msr_value){
  asm volatile ( "wrmsr" : : "c" (msr_id), "A" (msr_value) );
}

uint64_t rdmsr(uint32_t msr_id){
  uint64_t msr_value;
  asm volatile ( "rdmsr" : "=A" (msr_value) : "c" (msr_id));
  return msr_value;
}

bool are_interrupts_enabled(){
  unsigned long flags;
  asm volatile ( "pushf\n\t"
                 "pop %0"
                 : "=g" (flags) );
  return flags & (1 << 9);
}

void lidt(void* base, uint16_t size)
{
    struct
    {
        uint16_t length;
        uint32_t base;
    } __attribute__((packed)) IDTR;

    IDTR.length = size;
    IDTR.base = (uint32_t)base;
    asm volatile ( "lidt (%0)" : : "r"(&IDTR) );
}
