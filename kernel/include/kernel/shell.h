#ifndef SHELL_H
#define SHELL_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct shell_cmd {
  char *name;
  void (*func)(void);
} shell_cmd_t;

int shell(char *cmd);

#endif
