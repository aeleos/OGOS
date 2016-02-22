#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel/shell.h>
#include <kernel/time.h>
#include <kernel/tty.h>
#include <kernel/system.h>

extern int detect_cpu();

void cmd_help(){
  printf("""Help:\n \
-> cpu - print cpu info\n \
-> time - display the time\n \
-> clear - clear screen\n \
-> reboot - reboot the PC\n \
""");
}

void cmd_cpu(){
  detect_cpu();
}

void cmd_time(){
  print_time();
}

void cmd_clear()
{
    term_clear();
}

void cmd_reboot()
{
    term_clear();
    printf("System will go down and reboot...\n");
    reboot();
}


int cmd_limit = 5;

shell_cmd_t cmd_table[] = {
    {"help",  cmd_help},
    {"cpu",    cmd_cpu},
    {"time",  cmd_time},
    {"clear",  cmd_clear},
    {"reboot",  cmd_reboot},
    //{"time",  cmd_time},

};

int shell(char *cmd)
{
    for (int i = 0; i < cmd_limit; i++) {
        if (!strcmp(cmd, cmd_table[i].name)) {
            cmd_table[i].func();
            return 0;
        }
    }
    return 1;
}
