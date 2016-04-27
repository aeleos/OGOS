#include <kernel/shell.h>

extern int detect_cpu();

void cmd_help(){
  printf("""Help:\n \
-> cpu - print cpu info\n \
-> time - display the time\n \
-> clear - clear screen\n \
-> reboot - reboot the PC\n \
-> memtest - kmalloc and free stress test\n \
-> memtestc - verbose memtest\n \
""");
}

void cmd_cpu(){
  detect_cpu();
}


void cmd_clear()
{
    tty_clear();
}

void cmd_reboot()
{
    tty_clear();
    printf("System will go down and reboot...\n");
    reboot();
}

void cmd_memtestv()
{
    malloc_test(1);
}

void cmd_memtest()
{
    malloc_test(0);
}

int cmd_limit = 6;

shell_cmd_t cmd_table[] = {
    {"help",  cmd_help},
    {"cpu",    cmd_cpu},
    {"clear",  cmd_clear},
    {"reboot",  cmd_reboot},
    {"memtest", cmd_memtest},
    {"memtestv", cmd_memtestv},
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
