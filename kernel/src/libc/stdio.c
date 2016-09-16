#include <libc/stdio.h>


extern void *stdin;
extern volatile int in_size;

int getch()
{
    while (true) {
        if (((char*)stdin)[in_size] != 0) {
            in_size++;
            break;
        }
    }
    int c = ((char*)stdin)[in_size - 1];
    return c;
}

int putchar(int ic)
{
  char c = (char) ic;
  tty_write(&c, sizeof(c));
  return ic;
}


int getchar()
{
    int c = getch();
    putchar(c);

    return c;
}

char *gets(char *str)
{
    int c = getch();
    int i = 0;
    while (c != '\n') {
        if (c != '\b') {
            str[i++] = c;
            putchar(c);
        } else if (c == '\b' && i > 0) {
            str[--i] = 0;
            putchar(c);
        }
        c = getch();
    }
    str[i] = '\0';
    putchar('\n');
    return str;
}


static void print(const char* data, size_t data_length)
{
 for (size_t i = 0; i < data_length; i++)
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

int puts(const char* string)
{
 return printf("%s\n", string);
}
