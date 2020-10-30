#include <stdio.h>
#include <system.h>
#include <time.h>


char *monthNames[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

volatile int64 systemTime = 0;

void setTime(int64 time) {
	systemTime = time;
	HumanTime hTime = getHumanTime();
	printf("Set system time to: %s %i %i %02i:%02i:%02i %s\n", monthNames[hTime.month-1], hTime.day, hTime.year, hTime.hours % 12, hTime.minutes, hTime.seconds, hTime.hours > 12 ? "PM" : "AM");
}

void timeTick() {
	systemTime++;
}

void setTimeFromHuman(HumanTime time) {
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
	setTime(calculatedValue);
}

int64 getTime() {
	return systemTime;
}

int8 timeZoneOffset = 0; //-6; //-7;

HumanTime getHumanTime() {
	HumanTime returnTime = {0};

	uint8 daysPerMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	int64 time = systemTime + timeZoneOffset*3600;

	int second = time % 60;
	int minute = (time / 60) % 60;
	int hour = (time / 60 / 60) % 24;

	int daysSince = time / 60 / 60 / 24;
	int yearsSince = time / 60 / 60 / 24 / 365.25;
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