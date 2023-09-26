#include <stdio.h>
#include <system.h>
#include <time/pit.h>
#include <time/rtc.h>
#include <time/time.h>

const char *monthNames[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
const char *weekDayNames[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

const int daysPerMonth[2][12] = {
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, // Normal year
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}  // Leap year
};

#define SECONDS_PER_DAY    86400 // 60 * 60 * 24
#define SECONDS_PER_HOUR   3600  // 60 * 60
#define SECONDS_PER_MINUTE 60    // 60

#define DAYS_PER_NORMAL_YEAR 365
#define DAYS_PER_LEAP_YEAR   366

volatile time_t systemTime = 0;

volatile uint64 RTC_ticks = 0;
volatile uint32 RTC_tps = 0;

volatile uint64 PIT_ticks = 0;
volatile uint32 PIT_tps = 0;

bool isLeapYear(uint32 year) {
	return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

void setTime(time_t time) {
	systemTime = time;
	printf("Set system time to: ");
	printHumanTime(time);
	printf(" (%lu)\n", time);
}

inline void PIT_Tick() {
	PIT_ticks++;
	// if (PIT_ticks % PIT_tps == 0) {

	// }
}

inline void RTC_Tick() {
	RTC_ticks++;
	if (RTC_ticks % RTC_tps == 0) {
		systemTime++;
	}
}

void initTime() {
	puts("Setting up PIT...");
	PIT_tps = 4096;
	PIT_Install(PIT_tps);
	puts("Setting up RTC...");
	RTC_tps = 1024;
	RTC_install();
}

void printHumanTime(time_t time) {
	HumanTime hTime = getHumanTime(time);
	printf("%s, %s %i %u %i:%02i:%02i %s", weekDayNames[hTime.weekday - 1], monthNames[hTime.month - 1], hTime.day, hTime.year, hTime.hours > 12 ? hTime.hours - 12 : hTime.hours, hTime.minutes, hTime.seconds, hTime.hours >= 12 ? "PM" : "AM");
}

time_t getTimeFromHuman(HumanTime time) {
	time_t calculatedValue = 0;
	for (uint8 i = 0; i < (time.year - 1970); i++) {
		if (isLeapYear(1970 + i)) {
			calculatedValue += 366 * SECONDS_PER_DAY;
		} else {
			calculatedValue += 365 * SECONDS_PER_DAY;
		}
	}

	bool leap = isLeapYear(time.year);

	for (uint8 i = 0; i < time.month - 1; i++) {
		calculatedValue += daysPerMonth[leap][i] * SECONDS_PER_DAY;
	}

	calculatedValue += (time.day - 1) * SECONDS_PER_DAY;

	calculatedValue += time.hours * SECONDS_PER_HOUR;
	calculatedValue += time.minutes * SECONDS_PER_MINUTE;
	calculatedValue += time.seconds;
	return calculatedValue;
}

time_t getTime() {
	return systemTime;
}

int8 timeZoneOffset = 0; //-7; //-6;

HumanTime getHumanTime(time_t timestamp) {
	HumanTime returnTime = {0};

	timestamp += timeZoneOffset * 3600;

	int second = timestamp % 60;
	int minute = timestamp / SECONDS_PER_MINUTE % 60;
	int hour = timestamp / SECONDS_PER_HOUR % 24;

	int daysSinceEpoch = timestamp / SECONDS_PER_DAY;

	int dayOfYear = 0;
	uint32 year = 1970;
	int remainingDays = daysSinceEpoch;
	while (true) {
		bool leap = isLeapYear(year);
		if (leap && remainingDays >= DAYS_PER_LEAP_YEAR) {
			remainingDays -= DAYS_PER_LEAP_YEAR;
			year++;
		} else if (!leap && remainingDays >= DAYS_PER_NORMAL_YEAR) {
			remainingDays -= DAYS_PER_NORMAL_YEAR;
			year++;
		} else {
			dayOfYear = remainingDays;
			break;
		}
	}

	bool leap = isLeapYear(year);

	int dayOfWeek = (daysSinceEpoch + 3) % 7 + 1;

	int month = 0;
	int dayOfMonth = dayOfYear;
	for (; daysPerMonth[leap][month] <= dayOfMonth; month++) {
		dayOfMonth -= daysPerMonth[leap][month];
	}
	month++;
	dayOfMonth++;

	returnTime.seconds = second;
	returnTime.minutes = minute;
	returnTime.hours = hour;
	returnTime.day = dayOfMonth;
	returnTime.weekday = dayOfWeek;
	returnTime.month = month;
	returnTime.year = year;

	return returnTime;
}

// for these functions we use the PIT as it is the most accurate timer we have

uint64 getUptimeSeconds() {
	return PIT_ticks / PIT_tps;
}
uint64 getUptimeMs() {
	return (PIT_ticks * 1000) / PIT_tps;
}

void wait(uint64 ms) {
	uint64 eticks = PIT_ticks + ((ms * PIT_tps) / 1000);
	while (PIT_ticks < eticks) {
		asm volatile("hlt");
	}
	return;
}
