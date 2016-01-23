#include <stdio.h>
#include <string.h>
#include <stdbool.h>
//#include <sys/cdefs.h>
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
