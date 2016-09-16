#include <libc/stdlib.h>
#include <libc/stdio.h>
#include <libc/ctype.h>

__attribute__((__noreturn__))
void abort()
{
	// TODO: Add proper kernel panic.
	printf("Kernel Panic: abort()\n");
	while (1) {}
	__builtin_unreachable();
}

int atoi(const char* s) {
	int n = 0;
	int neg = 0;

	while (isspace(*s)) {
		s++;
	}

	switch (*s) {
		case '-':
			neg = 1;
			/* Fallthrough is intentional here */
		case '+':
			s++;
	}

	while (isdigit(*s)) {
		n = 10*n - (*s++ - '0');
	}
	/* The sign order may look incorrect here but this is correct as n is calculated
	 * as a negative number to avoid overflow on INT_MAX.
	 */
	return neg ? n : -n;
}

char* itoa(int value, char* str, int base)
{
    char* rc;
    char* ptr;
    char* low;

    if (base < 2 || base > 36) {
        *str = '\0';
        return str;
    }

    rc = ptr = str;
    if (value < 0 && base == 10) {
        *ptr++ = '-';
    }

    low = ptr;

    do {
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while (value);

    *ptr-- = '\0';

    while (low < ptr) {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }

    return rc;
}
