#ifndef TTY_H
#define TTY_H

#include <kernel/system.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

typedef enum {
	COLOR_BLACK,
	COLOR_BLUE,
	COLOR_GREEN,
	COLOR_CYAN,
	COLOR_RED,
	COLOR_MAGENTA,
	COLOR_BROWN,
	COLOR_LIGHT_GREY, // Max background color
	COLOR_DARK_GREY,
	COLOR_LIGHT_BLUE,
	COLOR_LIGHT_GREEN,
	COLOR_LIGHT_CYAN,
	COLOR_LIGHT_RED,
	COLOR_LIGHT_MAGENTA,
	COLOR_LIGHT_BROWN,
	COLOR_WHITE,
} vga_color;

typedef struct {
  uint16_t entry;
  uint8_t c;
  uint8_t color;
  uint8_t color_fg;
  uint8_t color_bg;
} entry_struct_t;


void tty_init();
void tty_clear();
void tty_menu_clear();
void tty_move_cursor();
void tty_print_entry_info(int32_t x, int32_t);
void tty_set_cursor(uint32_t row, uint32_t col);
void tty_putchar_at(char c, size_t x, size_t y);
void tty_setcolor(vga_color fg, vga_color bg);
void tty_change_bg_color(vga_color bg);
void tty_set_blink(int blink);
void tty_scrolldown();
void tty_putchar(char c);
void tty_write(const char* data, size_t size);
void tty_writestring(const char* data);
int tty_interpret_ansi(char c);
void tty_arrow(uint8_t sc);

#endif
