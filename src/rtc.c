#include <io.h>
#include <irq.h>
#include <libs.h>
#include <stdio.h>
#include <system.h>
#include <time.h>

#define cmos_address 0x70
#define cmos_data 0x71

#define update_in_progress_flag 0x80

#define seconds_register 0x00
#define minutes_register 0x02
#define hours_register 0x04
#define weekday_register 0x06
#define day_of_month_register 0x07
#define month_register 0x08
#define year_register 0x09
#define century_register 0x32
#define status_register_a 0x0A
#define status_register_b 0x0B

bool NMIsEnabled = true;

bool isRTCUpdating() {
      outb(cmos_address, 0x0A);
      return (inb(cmos_data) & update_in_progress_flag);
}
uint8 readCMOSRegister(uint8 reg) {
	outb(cmos_address, (!NMIsEnabled << 7) | (reg));
	return inb(cmos_data);
}

void rtc_install() {
	while (isRTCUpdating()) {}
	uint8 statusRegisterBValue = readCMOSRegister(status_register_b);

	bool twentyFourHourMode = statusRegisterBValue & 0x02;
	bool binaryMode = statusRegisterBValue & 0x04;

	uint8 seconds = readCMOSRegister(seconds_register);
	uint8 minutes = readCMOSRegister(minutes_register);
	uint8 hours = readCMOSRegister(hours_register);
	uint8 day = readCMOSRegister(day_of_month_register);
	uint8 month = readCMOSRegister(month_register);
	uint8 halfYear = readCMOSRegister(year_register);
	uint8 century = readCMOSRegister(century_register);
	uint32 year = 0;

	// printf("24 Hour mode: %i, Binary mode: %i\n", twentyFourHourMode, binaryMode);

	if (!binaryMode) {
		seconds = bcdToDecimal(seconds);
		minutes = bcdToDecimal(minutes);
		hours = bcdToDecimal(hours);
		day = bcdToDecimal(day);
		month = bcdToDecimal(month);
		halfYear = bcdToDecimal(halfYear);
		century = bcdToDecimal(century);
	}

	if (!twentyFourHourMode && hours & 0x80) {
		hours = (hours & 0x7f) + 12;
	}

	year = (century * 100) + halfYear;

	HumanTime time = {0};

	time.seconds = seconds;
	time.minutes = minutes;
	time.hours = hours;
	time.day = day;
	time.month = month;
	time.year = year;

	setTimeFromHuman(time);

	// printf("%i/%i %i %i:%i:%i\n", month, day, year, hours, minutes, seconds);
}



