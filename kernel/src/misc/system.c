#include <kernel/system.h>

void panic(const char *msg, int line, char *file)
{
	CLI();
	printf("[KERNEL PANIC]");
	printf("%s. Error at line %d in file %s.", msg, line, file);
	keep_running();
}

void reboot()
{
	uint8_t good = 0x02;
	while (good & 0x02)
		good = inportb(0x64);
	outportb(0x64, 0xFE);
	HLT();
}

static unsigned long int next = 1;

int rand( void ) // RAND_MAX assumed to be 32767
{
    next = next * 1103515245 + 12345;
    return (unsigned int)(next / 65536) % 32768;
}

void srand( unsigned int seed )
{
    next = seed;
}
