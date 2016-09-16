#ifndef ASSERT_H
#define ASSERT_H

#ifdef NDEBUG
#define assert(expr)
#else
#include <libc/stdio.h>
#include <libc/stdlib.h>
#define STR(x) #x
#define assert(expr) \
	do { \
		if (!(expr)) { \
			printf("Assertion failed: %s, file %s, line %d\n", STR(expr), __FILE__, __LINE__); \
			abort(); \
		} \
	} while (0)
#endif

#endif
