#include <kernel/time.h>
#include <kernel/isr.h>
#include <kernel/irq.h>
#include <kernel/com.h>
#include <kernel/tty.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


static unsigned int ticks = 0;

static unsigned long long time_us = 0;
static int century_register = 0x32; // ((TODO)) Check on older PCs

const unsigned short mdays[13] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
const unsigned short mdays_leap[13] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };


unsigned long long ttime_computeDaysForYear(unsigned long long year) {
 unsigned long long ret = 0;
 unsigned long long i = 1970;
 for(; i < year; i++) {
	 if(i % 4 == 0) {
		 ret += 366;
	 } else {
		 ret += 365;
	 }
 }
 return ret;
}


/*
 Getting & Parsing time and date
 */
unsigned long long ttime() {
	 TDate date;
	 unsigned long long tim;
	 int retv = tlibs_queryTime(&date, &tim);
	 if(retv & 0x02) {
		 return tim;
	 } else if(retv & 0x01) {
		 return ttime_parseDate(&date);
	 }
	 return 0;
}

void ttime_sleep(unsigned long long us) {

	 unsigned long long waitto = (ttime() + us);
	 while(ttime() < waitto);
}

unsigned long long ttime_parseDate(TDate* dt) {
 return ttime_computeTime(dt->us, dt->sec, dt->min, dt->hour, dt->day, dt->month, dt->year);
}


void tdate(TDate* date) {
	 unsigned long long tim;
	 int retv = tlibs_queryTime(date, &tim);
	 if(retv & 0x01) {
		 return;
	 } else if(retv & 0x02) {
		 tdate_parseTime(date, tim);
		 return;
	 }
	 memset(date, 0, sizeof(TDate));
}

void tdate_parseTime(TDate* dt, unsigned long long t) {
 /*
	YOLO ON
	((TODO))
	*/
 dt->us = (t % 1000000);
 t /= 1000000;

 unsigned long long epochDays = t / (24 * 60 * 60);
 unsigned int daySecs = t % (24 * 60 * 60);
 unsigned int sec = daySecs % 60;
 unsigned int min = (daySecs % 3600) / 60;
 unsigned int hour = daySecs / 3600;
 unsigned long long epochYears = (epochDays - ((epochDays + 365) / 1460)) / 365;
 unsigned int yearDay = (unsigned int) ((unsigned long long) epochDays -
		 (unsigned long long) ((epochYears * 365) + (epochYears + 1) / 4));
 unsigned long long year = 1970 + epochYears;

 const unsigned short* mstart = ((year & 0x03) ? mdays : mdays_leap);
 unsigned char month = 1;
 while(yearDay >= mstart[month]) {
	 month++;
 }
 unsigned int day = (1 + yearDay - (mstart[month - 1]));
 unsigned int weekDay = (epochDays + 4) % 7;

 dt->sec = (unsigned char) sec;
 dt->min = (unsigned char) min;
 dt->hour = (unsigned char) hour;
 dt->day = (unsigned char) day;
 dt->month = (unsigned char) month;
 dt->year = (unsigned long long) year;
 dt->yday = (unsigned short) yearDay;
 dt->wday = (unsigned char) weekDay;
 /*
	YOLO OFF
	*/
}


static void rtc_handler(registers_t* regs) {
//	(void) regs;
	ticks++;
	if(ticks == RTC_FREQ) {
		//One more second elapsed
    //printf("%d", ticks);

		ticks = 0;
		time_us += 1000000;
		//We will print current date
		TDate dat;
		tdate(&dat);

	//printf("+1 second... %02u. %02u. %llu, %02u:%02u:%02u\n", dat.day, dat.month,
	//			(unsigned long long) dat.year, dat.hour, dat.min, dat.sec);
		printf("\x1B[s\x1B[0;60H"); // save & move cursor
  	printf("\x1B[1K");

	  printf("\x1B[3m\x1B[37m\x1B[40m%d-%d-%d  ", dat.month, dat.day, (unsigned long long)dat.year);


    if (dat.hour < 10){
      printf("0");
    }
      printf("%d:", dat.hour);

    if (dat.min < 10){
      printf("0");
    }
    printf("%d:", dat.min);


    if (dat.sec < 10) {
      printf("0");
    }
    printf("%d", dat.sec);


	  printf("\x1B[4m\x1B[u");            // Restore cursor
    move_cursor();
	}


	outportb(0x70, 0x0C);
	(volatile void) inportb(0x71);
}


static inline unsigned char get_update_in_progress_flag() {
	outportb(0x70, 0x0A);
	return (inportb(0x71) & 0x80);
}

static inline unsigned char get_RTC_register(unsigned char reg) {
	outportb(0x70, reg);
	return inportb(0x71);
}


void time_install() {
	/*
	 Read Date
	 And convert it to time

	 */
	{
		unsigned int second = 0;
		unsigned int minute = 0;
		unsigned int hour = 0;
		unsigned int day = 0;
		unsigned int month = 0;
		unsigned int year = 0;

		unsigned char century;
		unsigned char last_second;
		unsigned char last_minute;
		unsigned char last_hour;
		unsigned char last_day;
		unsigned char last_month;
		unsigned char last_year;
		unsigned char last_century;
		unsigned char registerB;

		/*
		 Note: This uses the "read registers until you get the same values
		       twice in a row" technique to avoid getting dodgy/inconsistent
		       values due to RTC updates
		 */

		/*
		 Make sure an update isn't in progress
		 */
		while(get_update_in_progress_flag());

		second = get_RTC_register(0x00);
		minute = get_RTC_register(0x02);
		hour = get_RTC_register(0x04);
		day = get_RTC_register(0x07);
		month = get_RTC_register(0x08);
		year = get_RTC_register(0x09);

		if(century_register != 0) {
			century = get_RTC_register(century_register);
		}

		do {
			last_second = second;
			last_minute = minute;
			last_hour = hour;
			last_day = day;
			last_month = month;
			last_year = year;
			last_century = century;

			/*
			 Make sure an update isn't in progress
			 */
			while(get_update_in_progress_flag());

			second = get_RTC_register(0x00);
			minute = get_RTC_register(0x02);
			hour = get_RTC_register(0x04);
			day = get_RTC_register(0x07);
			month = get_RTC_register(0x08);
			year = get_RTC_register(0x09);

			if(century_register != 0) {
				century = get_RTC_register(century_register);
			}
		} while((last_second != second) || (last_minute != minute) ||
				(last_hour != hour) || (last_day != day) ||
				(last_month != month) || (last_year != year) ||
				(last_century != century));

		registerB = get_RTC_register(0x0B);

		/*
		 Convert BCD to binary values if necessary
		 */
		if(!(registerB & 0x04)) {
			second = (second & 0x0F) + ((second / 16) * 10);
			minute = (minute & 0x0F) + ((minute / 16) * 10);
			hour = (((hour & 0x0F) + (((hour & 0x70) / 16) * 10)) |
					(hour & 0x80));
			day = (day & 0x0F) + ((day / 16) * 10);
			month = (month & 0x0F) + ((month / 16) * 10);
			year = (year & 0x0F) + ((year / 16) * 10);
			if(century_register != 0) {
				century = (century & 0x0F) + ((century / 16) * 10);
			}
		}

		/*
		 Convert 12 hour clock to 24 hour clock if necessary
		 */
		if (!(registerB & 0x02) && (hour & 0x80)) {
			hour = ((hour & 0x7F) + 12) % 24;
		}

		/*
		 Calculate the full (4-digit) year
		 */
		if(century_register != 0) {
			year += (century * 100);
		} else {
			year += RTC_PLUSCENT;
		}



		/*
		 Compute Time
		 */
		TDate dat;
		dat.us = 0;
		dat.sec = second;
		dat.min = minute;
		dat.hour = hour;
		dat.day = day;
		dat.month = month;
		dat.year = year;
		dat.wday = ttime_computeWeekDayFromDays(day + ttime_computeDaysForMonthWithYear(month, year));
		dat.yday = ttime_computeYearDayFromCurDaysAndMonthWithoutYear(day, month, year);

		time_us = ttime_parseDate(&dat);
	}


	/*
	 Setup RTC
	 */

	irq_register_handler(IRQ8, &rtc_handler);

	outportb(0x70, 0x8A);	// select Status Register A, and disable NMI (by setting the 0x80 bit)
	outportb(0x71, 0x20);	// write to CMOS/RTC RAM

	outportb(0x70, 0x8B);		// select register B, and disable NMI
	char prev = inportb(0x71);	// read the current value of register B
	outportb(0x70, 0x8B);		// set the index again (a read will reset the index to register D)
	outportb(0x71, (prev | 0x40));	// write the previous value ORed with 0x40. This turns on bit 6 of register B

	outportb(0x70, 0x8A);		// set index to register A, disable NMI
	prev = inportb(0x71);	// get initial value of register A
	outportb(0x70, 0x8A);		// reset index to A
	outportb(0x71, (prev & 0xF0) | RTC_RATE); //write only our rate to A. Note, rate is the bottom 4 bits.

}


int tlibs_queryTime(TDate* date, unsigned long long* tim) {
	(void) date;

	unsigned long long curtim = time_us;
	curtim += (unsigned long long) ((double) ticks * (double) RTC_US);
	*tim = curtim;

	return 0x02; //Only time set
}
