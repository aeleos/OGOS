#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <kernel/tty.h>
#include <kernel/com.h>

#define ENTRY(x, y) tty_buffer[(y)*VGA_WIDTH+(x)]

static int32_t tty_row;
static int32_t tty_column;
static uint8_t tty_color;
static uint8_t tty_menu_color;
static uint16_t* tty_buffer;



// Helper functions
static uint8_t tty_make_color(vga_color fg, vga_color bg) {
	return fg | bg << 4;
}

// Entry format: [ BLINK BG BG BG FG FG FG FG | C C C C C C C C ]
//               [           8-bits           |      8-bits     ]
static uint16_t tty_make_entry(char c, uint8_t color) {
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}

static uint8_t tty_get_fg_color() {
	return tty_color & 0x000F;
}

static uint8_t tty_get_bg_color() {
	return tty_color >> 4;
}

static void tty_set_bg_color(vga_color color) {
	tty_color = tty_make_color(tty_get_fg_color(), color);
}

static void tty_set_fg_color(vga_color color) {
	tty_color = tty_make_color(color, tty_get_bg_color());
}


static uint16_t get_entry(int32_t x, int32_t y){
	return (uint16_t)tty_buffer[(y*VGA_WIDTH)+x];
}

static entry_struct_t get_entry_info(uint16_t entry16){
	entry_struct_t entry_struct;
	entry_struct.entry = entry16;
	entry_struct.c = entry16 & 0xFF;
	entry_struct.color  = entry16 >> 8;
	entry_struct.color_fg = (entry16 >> 8) & 0x000F;
	entry_struct.color_bg = (entry16 >> 8) >> 4;
	return entry_struct;
}

void tty_print_entry_info(int32_t x, int32_t y){
	uint16_t entry = get_entry(x, y);
	entry_struct_t entry_struct = get_entry_info(entry);
	tty_putchar(entry_struct.c);
	printf("%X", entry_struct.color);
	printf("%X", entry_struct.color_fg);
	printf("%X", entry_struct.color_bg);
	return;
}

// ttyinal handling functions
void tty_init() {
	uint16_t entry = tty_make_entry(' ', tty_color);
	tty_buffer = (uint16_t*) VGA_MEMORY;
	tty_color = tty_make_color(COLOR_WHITE, COLOR_BLACK);
	tty_set_cursor(0, 0);


	for (uint32_t x = 0; x < VGA_WIDTH; x++) {
		for (uint32_t y = 0; y < VGA_HEIGHT; y++) {
			ENTRY(x, y) = entry;
		}
	}
	tty_set_cursor(1, 0);

}

void tty_clear(){
	tty_init();
	tty_change_bg_color(COLOR_DARK_GREY);
	tty_menu_clear();
}

void tty_set_cursor(uint32_t row, uint32_t col){
	tty_row = row;
	tty_column = col;
	tty_move_cursor();
}

void tty_menu_clear(){
	tty_menu_color = tty_make_color(COLOR_WHITE, COLOR_BLACK);
	uint16_t menu_entry = tty_make_entry(' ', tty_menu_color);
	for (uint32_t x = 0; x < VGA_WIDTH; x++){
		ENTRY (x, 0) = menu_entry;
	}
}

void tty_setcolor(vga_color fg, vga_color bg) {
	tty_color = tty_make_color(fg, bg);
}

void tty_change_bg_color(vga_color bg) {
	for (uint32_t x = 0; x < VGA_WIDTH; x++) {
		for (uint32_t y = 1; y < VGA_HEIGHT; y++) {
			uint16_t entry = ENTRY(x, y);
			char c = entry & 0xFF;
			vga_color fg = (entry & 0x0F00) >> 8;
			ENTRY(x, y) = tty_make_entry(c, tty_make_color(fg, bg));
		}
	}

	tty_set_bg_color(bg);
}

void tty_set_blink(int blink) {
	if (blink) {
		tty_color |= (1 << 7);
	}
	else {
		tty_color &= ~(1 << 7);
	}
}



void tty_move_cursor()
{
   // The screen is 80 characters wide...
	const size_t index = tty_row * VGA_WIDTH + tty_column;

	 // cursor LOW port to vga INDEX register
	outportb(0x3D4, 14); // Tell the VGA board we are setting the high cursor byte.
 	outportb(0x3D5, index >> 8); // Send the high cursor byte.
 	outportb(0x3D4, 15); // Tell the VGA board we are setting the low cursor byte.
 	outportb(0x3D5, index); // Send the low cursor byte.


}

void tty_putchar_at(char c, uint32_t x, uint32_t y) {
	if (y >= VGA_HEIGHT || x >= VGA_WIDTH) {
		return;
	}

	ENTRY(x, y) = tty_make_entry(c, tty_color);
}

void tty_scrolldown() {
	for (uint32_t y = 1; y < VGA_HEIGHT; y++) {
		for (uint32_t x = 0; x < VGA_WIDTH; x++) {
			if (y < VGA_HEIGHT-1) {
				ENTRY(x, y) = ENTRY(x, y+1);
			}
			else { // last line
				ENTRY(x, y) = tty_make_entry(' ', tty_color);
			}
		}
	}
	tty_set_cursor(tty_row - 1, tty_column);
	//tty_row--;
}


void tty_arrow(uint8_t sc){
	//printf("%X", sc);
	uint8_t left = 0x4B;
	uint8_t right = 0x4D;
	uint8_t up = 0x48;
	uint8_t down = 0x50;
	if (sc == left)
	{
		tty_set_cursor(tty_row, tty_column - 1);
		if (tty_column < 0){
			tty_set_cursor(tty_row - 1, 78);
		}
	} else if (sc == right){
		tty_set_cursor(tty_row, tty_column + 1);
	} else if (sc == up){
		tty_set_cursor(tty_row - 1, tty_column);
	} else if (sc == down){
		tty_set_cursor(tty_row + 1, tty_column);
	}
}

void tty_putchar(char c) {
	if (c == '\n' || c == '\r') {
		// tty_row += 1;
		// tty_column = 0;
		tty_set_cursor(tty_row + 1, 0);
	}
	else if (c == '\t') {
		for (int i = 0; i < 4; i++) {
			tty_putchar_at(' ', tty_column++, tty_row);
			tty_move_cursor();
		}
	}
	else if (c == '\b'){
		tty_set_cursor(tty_row, tty_column - 1);
	 	if (tty_column < 0){
			tty_set_cursor(tty_row - 1, 78);
		}
		tty_putchar_at(' ', tty_column, tty_row);
	}
	/*
	else if (c == '\a'){
		tty_set_cursor(tty_row, tty_column - 1);
		if (tty_column < 0){
			tty_set_cursor(tty_row - 1, 78);
		}
	}
	else if (c == '\d'){
		tty_set_cursor(tty_row, tty_column + 1);
	}
	else if (c == '\w'){
		tty_set_cursor(tty_row - 1, tty_column);
	}
	else if (c == '\s'){
		tty_set_cursor(tty_row + 1, tty_column);
	}
	*/



	if (tty_column >= VGA_WIDTH) {
		// tty_row += 1;
		// tty_column = 0;
		tty_set_cursor(tty_row + 1, 0);

	}

	if (tty_row >= VGA_HEIGHT) {
		tty_scrolldown();
	}

	if (tty_interpret_ansi(c) || !isprint(c)) {
		return;
	}

	tty_putchar_at(c, tty_column++, tty_row);
	tty_move_cursor();
}

void tty_write(const char* data, uint32_t size) {
	for (uint32_t i = 0; i < size; i++) {
		tty_putchar(data[i]);
	}
}



void tty_write_string(const char* data) {
	tty_write(data, strlen(data));
}



// Returns != 0 if in an ANSI sequence
// ANSI escape sequence: \x1B[param;param2...end_char
int tty_interpret_ansi(char c) {
	typedef enum {
		normal, bracket, params
	} state_t;

	static state_t state = normal;     // State tracker
	static char buf[32] = "";          // Stores the string for 1 param
	static uint32_t args[32] = { 0 };  // Stores the parsed params
	static uint32_t current_arg = 0;   // Number of params
	static uint32_t current_index = 0; // Number of chars in current param
	static uint32_t saved_row = 0;
	static uint32_t saved_col = 0;
	static uint8_t saved_color;
	static uint8_t saved_menu_color;


	if (state == normal) {
		if (c == 0x1B) { // Escape character
			state = bracket;
		}
		else {
			return 0;
		}
	}
	else if (state == bracket) {
		if (c == '[') {
			state = params;
		}
		else {
			state = normal;
			return 0;
		}
	}
	else if (state == params) {
		if (c == ';') {
			buf[current_index] = '\0';
			args[current_arg++] = atoi(buf);
			current_index = 0;
		}
		else if (isdigit(c)) {
			if (current_index >= 32) {
				current_arg = 0;
				current_index = 0;
				state = normal;
			}
			else {
				buf[current_index++] = c;
			}
		}
		else if (isalpha(c)) {
			buf[current_index] = '\0';
			args[current_arg++] = atoi(buf);

			switch (c) {
				case 's': // Save cursor position
					saved_row = tty_row;
					saved_col = tty_column;
					state = normal;
					break;
				case 'u': // Restore cursor position
					tty_row = saved_row;
					tty_column = saved_col;
					state = normal;
					break;
				case 'K': // Erase until the end of line
					for (uint32_t x = tty_column; x < VGA_WIDTH; x++) {
						if (args[0]){
							ENTRY(x, tty_row) = tty_make_entry(' ', tty_menu_color);
						} else {
							ENTRY(x, tty_row) = tty_make_entry(' ', tty_color);
						}
					}
					state = normal;
					break;
				case 'H': // Set cursor position
				case 'f':
					tty_row = args[0];
					tty_column = args[1];
					break;
				case 'A': // Cursor up
					tty_row -= args[0]; break;
				case 'B': // Cursor down
					tty_row += args[0]; break;
				case 'C': // Cursor right
					tty_column += args[0]; break;
				case 'D': // Cursor left
					tty_column -= args[0]; break;
				case 'J': // 2J: clear screen & reset cursor
					if (args[0] == 2) {
						tty_init();
					}
					break;
			}

			if (c == 'm') { // Set graphics mode
				for (uint32_t i = 0; i < current_arg; i++) {
					switch (args[i]) {
						case 0:
							//tty_set_blink(0); break;
							tty_move_cursor(); break;
						case 1: saved_color = tty_color; break;//save tty color
						case 2: tty_color = saved_color; break;//restore tty color
						case 3: saved_menu_color = tty_menu_color; saved_color = tty_color; break;//save menu color
						case 4: tty_menu_color = saved_menu_color; tty_color = saved_color; break;//restore menu color
						case 5:
							//tty_set_blink(1); break;
							tty_move_cursor(); break;
						case 30:
							tty_set_fg_color(COLOR_BLACK); break;
						case 31:
							tty_set_fg_color(COLOR_RED); break;
						case 32:
							tty_set_fg_color(COLOR_GREEN); break;
						case 33: // Yellow
							tty_set_fg_color(COLOR_BROWN); break;
						case 34:
							tty_set_fg_color(COLOR_BLUE); break;
						case 35:
							tty_set_fg_color(COLOR_MAGENTA); break;
						case 36:
							tty_set_fg_color(COLOR_CYAN); break;
						case 37:
							tty_set_fg_color(COLOR_WHITE); break;
						case 40:
							tty_set_bg_color(COLOR_BLACK); break;
						case 41:
							tty_set_bg_color(COLOR_RED); break;
						case 42:
							tty_set_bg_color(COLOR_GREEN); break;
						case 43: // Yellow
							tty_set_bg_color(COLOR_BROWN); break;
						case 44:
							tty_set_bg_color(COLOR_BLUE); break;
						case 45:
							tty_set_bg_color(COLOR_MAGENTA); break;
						case 46:
							tty_set_bg_color(COLOR_CYAN); break;
						case 47: // White
							tty_set_bg_color(COLOR_LIGHT_GREY); break;
					}
				}
			}

			current_arg = 0;
			current_index = 0;
			state = normal;
		}
	}

	return 1;
}
