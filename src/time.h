#include <system.h>

#pragma once

typedef struct HumanTime {
	uint8 seconds;
	uint8 minutes;
	uint8 hours;
	uint8 day;
	uint8 weekday;
	uint8 month;
	uint32 year;
} HumanTime;

typedef int64 time_t;

void setTime(time_t time);

void PIT_Tick();

void RTC_Tick();

void initTime();

void printHumanTime();

time_t getTimeFromHuman(HumanTime time);

time_t getTime();

HumanTime getHumanTime();

uint64 getUptimeSeconds();
uint64 getUptimeMs();

void wait(uint64 ms);