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

void timeTick();

void setTimeFromHuman(HumanTime time);

int64 getTime();

HumanTime getHumanTime();