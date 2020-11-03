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

void PIT_Tick();

void RTC_Tick();

void timeTick();

void initTime();

void printHumanTime();

void setTimeFromHuman(HumanTime time);

int64 getTime();

HumanTime getHumanTime();

long getUptime();
long getUptimeTicks();

void wait(uint32 ms);