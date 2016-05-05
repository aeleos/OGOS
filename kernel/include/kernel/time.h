#ifndef _TIME_H_
#define _TIME_H_

#include <kernel/system.h>



#define RTC_RATE 3
#define RTC_FREQ (0x8000 >> (RTC_RATE - 1))
#define RTC_US (1000000.0 / (double) (RTC_FREQ))
#define RTC_PLUSCENT 2000


typedef struct {
	unsigned long long year; //[1970+]
	unsigned int us; //[0-999'999]

	unsigned char sec; //[0-59]
	unsigned char min; //[0-59]
	unsigned char hour; //[0-23]
	unsigned char day; //[1-31]
	unsigned char wday; //[1-7]
	unsigned char month; //[1-12]
	unsigned short yday; //[1-7]
} __attribute__((packed)) TDate;

extern const unsigned short mdays[13]; // = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
extern const unsigned short mdays_leap[13]; // = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };




/*
 Setup RTC - call from main
 Query Time - call from TLibs
 */
void time_install();
int tlibs_queryTime(TDate* date, unsigned long long* tim);
unsigned long long ttime();
void ttime_sleep(unsigned long long us);
unsigned long long ttime_parseDate(TDate* dt);

void tdate(TDate* date);
void tdate_parseTime(TDate* dt, unsigned long long time);




unsigned long long ttime_computeDaysForYear(unsigned long long year);

#define ttime_computeWeekDayFromDays(days) \
	((days == 0) ? 6 : (days - 1) % 7)

#define ttime_computeDaysForMonthWithoutYear(month, year) \
	((year % 4 == 0) ? mdays_leap[month - 1] : mdays[month - 1])

#define ttime_computeYearDayFromCurDaysAndMonthWithoutYear(days, month, year) \
	(ttime_computeDaysForMonthWithoutYear(month, year) + (days - 1))

#define ttime_computeDaysForMonthWithYear(month, year) \
	(ttime_computeDaysForYear(year) + ttime_computeDaysForMonthWithoutYear(month, year))

#define ttime_computeTime(usecond, second, minute, hour, day, month, year)     \
	(unsigned long long) (                                                     \
		usecond +                                                              \
			((unsigned long long) (second +                                    \
				(minute +                                                      \
					(hour +                                                    \
						((day - 1) +                                           \
							ttime_computeDaysForMonthWithYear(month, year))    \
					* 24U)                                                     \
				* 60U)                                                         \
			* 60U))                                                            \
		* 1000000U                                                             \
	)







#endif
