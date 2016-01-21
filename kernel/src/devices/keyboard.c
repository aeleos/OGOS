#include <stdio.h>

#include <kernel/keyboard.h>
#include <kernel/irq.h>
#include <kernel/com.h>
#include <kernel/sys.h>

#include <kernel/tty.h>

uint8_t keyboard_buffer[512];
uint32_t buf_length;;

/*
char sc_to_char[] = {
	0, 0,
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
	'-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
	'[', ']', '\r', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
	';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
	',', '.', '/', 0, '*', 0, ' ',
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	'7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',
	0, 0, 0, 0, 0
};

*/
char sc_to_char[] = {
	0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
	'9', '0', '-', '=', '\b',	/* Backspace */
	'\t',			/* Tab */
	'q', 'w', 'e', 'r',	/* 19 */
	't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
	0,			/* 29   - Control */
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
	'\'', '`',   0,		/* Left shift */
	'\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
	'm', ',', '.', '/',   0,				/* Right shift */
	'*',
	0,	/* Alt */
	' ',	/* Space bar */
	0,	/* Caps lock */
	0,	/* 59 - F1 key ... > */
	0,   0,   0,   0,   0,   0,   0,   0,
	0,	/* < ... F10 */
	0,	/* 69 - Num lock*/
	0,	/* Scroll Lock */
	0,	/* Home key */
	0,	/* Up Arrow */
	0,	/* Page Up */
	'-',
	0,	/* Left Arrow */
	0,
	0,	/* Right Arrow */
	'+',
	0,	/* 79 - End key*/
	0,	/* Down Arrow */
	0,	/* Page Down */
	0,	/* Insert Key */
	0,	/* Delete Key */
	0,   0,   0,
	0,	/* F11 Key */
	0,	/* F12 Key */
	0,	/* All other keys are undefined */
};


char shift_sc_to_char[] = {
	0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
	'(', ')', '_', '+', '\b',	/* Backspace */
	'\t',			/* Tab */
	'Q', 'W', 'W', 'R',	/* 19 */
	'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	/* Enter key */
	0,			/* 29   - Control */
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	/* 39 */
	'"', '~',   0,		/* Left shift */
	'|', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
	'M', '<', '>', '?',   0,				/* Right shift */
	'*',
	0,	/* Alt */
	' ',	/* Space bar */
	0,	/* Caps lock */
	0,	/* 59 - F1 key ... > */
	0,   0,   0,   0,   0,   0,   0,   0,
	0,	/* < ... F10 */
	0,	/* 69 - Num lock*/
	0,	/* Scroll Lock */
	0,	/* Home key */
	0,	/* Up Arrow */
	0,	/* Page Up */
	'-',
	0,	/* Left Arrow */
	0,
	0,	/* Right Arrow */
	'+',
	0,	/* 79 - End key*/
	0,	/* Down Arrow */
	0,	/* Page Down */
	0,	/* Insert Key */
	0,	/* Delete Key */
	0,   0,   0,
	0,	/* F11 Key */
	0,	/* F12 Key */
	0,	/* All other keys are undefined */
};


volatile key_state_t keys_state[256];
volatile key_event_t key_event;
uint8_t keys_waiting_for[256];
uint8_t wait_for_key;
void init_keyboard(){
  irq_register_handler(IRQ1, &keyboard_handler);
}

void keyboard_handler(registers_t* regs){
  UNUSED(regs);

  uint8_t scancode = inportb(KBD_DATA);
  uint8_t sc = scancode & 0x7F;


  if (scancode & KEY_RELEASED){
    keys_state[sc].pressed = 0;
    key_event.pressed = 0;
  }
  else {
    keys_state[sc].pressed = 1;
    key_event.pressed = 1;
  }

	if (keys_waiting_for[sc] || wait_for_key){
		return;
	}

  key_event.key = sc;
  key_event.alt = keys_state[KEY_L_ALT].pressed;
  key_event.alt_gr = 0;
  key_event.shift = keys_state[KEY_L_SHIFT].pressed ||
					  keys_state[KEY_R_SHIFT].pressed;
  key_event.super = keys_state[KEY_SUPER].pressed;
  key_event.control = keys_state[KEY_L_CTRL].pressed;

  if (sc_to_char[scancode]) {
		if (key_event.shift){
			printf("%c", shift_sc_to_char[sc]);
		}
		else {
			printf("%c", sc_to_char[sc]);
		}
		/*,key_event.alt ? "Alt" : "",
    key_event.shift ? "Shift" : "",
    key_event.control ? "Control" : "",
    key_event.super ? "Super" : ""*/

  }
	else {
		term_arrow(scancode);
	}
}



void keyboard_wait_for_key(uint8_t key){
	keys_waiting_for[key] = 1;
	while(1){
		if (keys_state[key].pressed){
			keys_waiting_for[key] = 0;
			break;
		}
	}
}

void keyboard_wait_press(){
	printf("Press any key to continue...\n");
	wait_for_key = 1;
	while(1){
		if(key_event.pressed){
			wait_for_key = 0;
			break;
		}
	}
}

void keyboard_wait() {
	while (inportb(KBD_STATUS) & (1 << 1));
}
