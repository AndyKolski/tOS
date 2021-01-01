#include <pit.h>
#include <rtc.h>
#include <stdio.h>
#include <system.h>
#include <time.h>


char *monthNames[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
char *weekDayNames[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

volatile time_t systemTime = 0;

volatile uint64 RTC_ticks = 0;
volatile uint32 RTC_tps = 0;

volatile uint64 PIT_ticks = 0;
volatile uint32 PIT_tps = 0;

void setTime(time_t time) {
	systemTime = time;
	puts("Set system time to: ");
	printHumanTime();
	printf(" (%i)\n", time);
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
	puts("Setting up PIT...\n");
	PIT_tps = 4096;
	PIT_Install(PIT_tps);
	puts("Setting up RTC...\n");
	RTC_tps = 1024;
	RTC_install();
}

void printHumanTime() {
	HumanTime hTime = getHumanTime();
	printf("%s, %s %i %i %i:%02i:%02i %s", weekDayNames[hTime.weekday-1], monthNames[hTime.month-1], hTime.day, hTime.year, hTime.hours > 12 ? hTime.hours - 12 : hTime.hours, hTime.minutes, hTime.seconds, hTime.hours >= 12 ? "PM" : "AM");
}

time_t getTimeFromHuman(HumanTime time) {
	int64 calculatedValue = 0;
	for(uint8 i = 0; i < (time.year - 1970); i++){
		if ((1970 + i) % 4 == 0 && ((1970 + i) % 100 != 0 || (1970 + i) % 400 == 0)) {
			calculatedValue += 366 * 24 * 60 * 60;
		} else {
			calculatedValue += 365 * 24 * 60 * 60;
		}
	}

	uint8 daysPerMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	if (time.year % 4 == 0 && (time.year % 100 != 0 || time.year % 400 == 0)) {
		daysPerMonth[1] = 29;
	} else {
		daysPerMonth[1] = 28;
	}

	for(uint8 i = 0; i < time.month-1; i++){
		calculatedValue += daysPerMonth[i] * 24 * 60 * 60;
	}

	calculatedValue += (time.day-1) * 60 * 60 * 24;

	calculatedValue += time.hours * 60 * 60;
	calculatedValue += time.minutes * 60;
	calculatedValue += time.seconds;
	return calculatedValue;
}

time_t getTime() {
	return systemTime;
}

int8 timeZoneOffset = 0; //-7; //-6;

HumanTime getHumanTime() {
	HumanTime returnTime = {0};

	uint8 daysPerMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	time_t time = systemTime + timeZoneOffset*3600;

	int second = time % 60;
	int minute = (time / 60) % 60;
	int hour = (time / 60 / 60) % 24;

	int daysSince = time / 60 / 60 / 24;
	float yearsSince = time / 60.0 / 60 / 24 / 365.25;
	int dayOfYear = daysSince - yearsSince * 365.25 + 1;
	int year = 1970 + yearsSince;
	int weekday = (daysSince + 4) % 7;

	bool isLeapYear = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));

	if (isLeapYear) {
		daysPerMonth[1] = 29;
	} else {
		daysPerMonth[1] = 28;
	}

	int month = 0;
	int day = dayOfYear;
	for (; daysPerMonth[month] <= day; month++) {
		day -= daysPerMonth[month];
	}
	month++;
	day++;

	returnTime.seconds = second;
	returnTime.minutes = minute;
	returnTime.hours = hour;
	returnTime.day = day;
	returnTime.weekday = weekday;
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
    uint64 eticks = PIT_ticks + ((ms*PIT_tps)/1000);
    while(PIT_ticks < eticks) {}
    return;
}