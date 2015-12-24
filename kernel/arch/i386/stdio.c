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

	while ( *format != '\0' )
	{
		if ( *format != '%' )
		{
		print_c:
			amount = 1;
			while ( format[amount] && format[amount] != '%' )
				amount++;
			print(format, amount);
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format;

		if ( *(++format) == '%' )
			goto print_c;

		if ( rejected_bad_specifier )
		{
		incomprehensible_conversion:
			rejected_bad_specifier = true;
			format = format_begun_at;
			goto print_c;
		}

		if ( *format == 'c' )
		{
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			print(&c, sizeof(c));
		}
		else if ( *format == 's' )
		{
			format++;
			const char* s = va_arg(parameters, const char*);
			print(s, strlen(s));
		}
		else if ( *format == 'd' )
		{
			format++;
			int i = va_arg(parameters, int);
			char buf[33];
			itoa( i, buf, 10 );
			print(buf, strlen(buf));
		}
		else if ( *format == 'x' )
		{
			format++;
			int i = va_arg(parameters, int);
			char buf[33];
			itoa( i, buf, 16 );
			print(buf, strlen(buf));
		}
		else
		{
			goto incomprehensible_conversion;
		}
	}

	va_end(parameters);

	return written;
}

int putchar(int ic)
{
	char c = (char) ic;
	terminal_write(&c, sizeof(c));
	return ic;
}

int puts(const char* string)
{
	return printf("%s\n", string);
}

void outb(uint16_t port, uint8_t val){
  asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

void outw(uint16_t port, uint16_t val){
  asm volatile ( "outw %0, %1" : : "a"(val), "Nd"(port) );
}

void outl(uint16_t port, uint32_t val){
  asm volatile ( "outl %0, %1" : : "a"(val), "Nd"(port) );
}

uint8_t inb(uint16_t port){
  uint8_t ret;
  asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
  return ret;
}

uint16_t inw(uint16_t port){
  uint16_t ret;
  asm volatile ( "inw %1, %0" : "=a"(ret) : "Nd"(port) );
  return ret;
}

uint32_t inl(uint16_t port){
  uint32_t ret;
  asm volatile ( "inl %1, %0" : "=a"(ret) : "Nd"(port) );
  return ret;
}

void io_wait(void){
  asm volatile ( "jmp 1f\n\n"
                 "1:jmp 2f\n\t"
                 "2:" );
}
