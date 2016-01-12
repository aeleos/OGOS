#include <stdio.h>

char* itoa( int value, char * str, int base )
{
    char * rc;
    char * ptr;
    char * low;
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}


int putchar(int ic)
{
#ifdef _KERNEL_
	char c = (char) ic;
	terminal_write(&c, sizeof(c));
#else
	// TODO: You need to implement a write system call.
#endif
	return ic;
}


int puts(const char* string)
{
	return printf("%s\n", string);
}


void print(const char* data, size_t data_length)
{
	for ( size_t i = 0; i < data_length; i++ )
		putchar((int) ((const unsigned char*) data)[i]);
}

int printf(const char* restrict format, ...)
{
	va_list parameters;
	va_start(parameters, format);

	int written = 0;
	size_t amount;
	bool rejected_bad_specifier = false;
	char conversion_buf[128] = "";

	while (*format != '\0') {
		if (*format != '%') {
		print_c:
			amount = 1;
			while (format[amount] && format[amount] != '%')
				amount++;
			print(format, amount);
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format;

		if (*(++format) == '%')
			goto print_c;

		if (rejected_bad_specifier) {
		incomprehensible_conversion:
			rejected_bad_specifier = true;
			format = format_begun_at;
			goto print_c;
		}

		if (*format == 'c') {
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			print(&c, sizeof(c));
		}
		else if (*format == 's') {
			format++;
			const char* s = va_arg(parameters, const char*);
			print(s, strlen(s));
		}
		else if (*format == 'd') {
			format++;
			int i = va_arg(parameters, int);
			const char* s = itoa(i, conversion_buf, 10);
			print(s, strlen(s));
		}
		else if (*format == 'x') {
			format++;
			int i = va_arg(parameters, int);
			itoa(i, conversion_buf, 16);
			print(conversion_buf, strlen(conversion_buf));
		}
		else if (*format == 'X') {
			format++;
			int i = va_arg(parameters, int);
			itoa(i, conversion_buf, 16);
			for (size_t i = 0; i < strlen(conversion_buf); i++)
				conversion_buf[i] = toupper(conversion_buf[i]);
			print(conversion_buf, strlen(conversion_buf));
		}
		else if (*format == 'b') {
			// non standard
			format++;
			int i = va_arg(parameters, int);
			itoa(i, conversion_buf, 2);
			print(conversion_buf, strlen(conversion_buf));
		}
		else {
			goto incomprehensible_conversion;
		}
	}

	va_end(parameters);

	return written;
}

uint16_t inports(uint16_t port) {
	uint16_t rv;
	asm volatile ("inw %1, %0" : "=a" (rv) : "dN" (port));
	return rv;
}

void outports(uint16_t port, uint16_t data) {
	asm volatile ("outw %1, %0" : : "dN" (port), "a" (data));
}

uint32_t inportl(uint16_t port) {
	unsigned int rv;
	asm volatile ("inl %%dx, %%eax" : "=a" (rv) : "dN" (port));
	return rv;
}

void outportl(uint16_t port, uint32_t data) {
	asm volatile ("outl %%eax, %%dx" : : "dN" (port), "a" (data));
}

uint8_t inportb(uint16_t port) {
	unsigned char rv;
	asm volatile ("inb %1, %0" : "=a" (rv) : "dN" (port));
	return rv;
}

void outportb(uint16_t port, uint8_t data) {
	asm volatile ("outb %1, %0" : : "dN" (port), "a" (data));
}

void outportsm(uint16_t port, unsigned char * data, uint32_t size) {
	asm volatile ("rep outsw" : "+S" (data), "+c" (size) : "d" (port));
}

void inportsm(uint16_t port, unsigned char * data, uint32_t size) {
	asm volatile ("rep insw" : "+D" (data), "+c" (size) : "d" (port) : "memory");
}
