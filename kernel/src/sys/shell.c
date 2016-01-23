#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel/shell.h>

extern int detect_cpu();

void cmd_help(){
  printf("""Help:\n \
-> hw - print hardware info\n \
-> time - display the time\n \
-> free - display info about memory\n \
-> clear - clear screen\n \
-> reboot - reboot the PC\n \
""");
}

void cmd_cpu(){
  detect_cpu();
}


int cmd_limit = 3;

shell_cmd_t cmd_table[] = {
    {"help",  cmd_help},
    {"cpu",    cmd_cpu},
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
