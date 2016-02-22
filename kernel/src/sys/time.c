#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel/time.h>
#include <kernel/tty.h>

extern int tick;
int last_tick;
int mseconds;
int seconds;
int minutes;
int hours;


void init_time(){
  mseconds = 0;
  seconds = 0;
  minutes = 0;
  hours = 0;
}

void update_time(){
  mseconds++;
  if (mseconds == 1000){
    seconds++;
    mseconds = 0;
  }
  if (seconds == 60){
    minutes++;
    seconds = 0;
  }
  if (minutes == 60){
    hours++;
    minutes = 0;
    seconds = 0;
  }
  if (hours == 24){
    hours = 0;
  }
  if ((tick - last_tick) > 100){
    last_tick = tick;
  	printf("\x1B[s\x1B[0;68H"); // save & move cursor
  	printf("\x1B[1K");            // Clear line
	  printf("\x1B[3m\x1B[37m\x1B[40m%d:%d:%d:%d", hours, minutes, seconds, mseconds);
	  printf("\x1B[4m\x1B[u");            // Restore cursor
    move_cursor();
  }
}

void print_time()
{
	printf("Time: %d:%d:%d:%d ", hours, minutes, seconds, mseconds);
	if (hours < 12) {
		printf("am");
	} else {
		printf("pm");
	}
	printf("\n");
}
