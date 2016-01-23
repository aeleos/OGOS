#include <stdio.h>
#include <string.h>

int getchar()
{
    int c = getch();
    putchar(c);

    return c;
}
