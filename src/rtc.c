#include <io.h>
#include <irq.h>
#include <libs.h>
#include <stdio.h>
#include <system.h>
#include <time.h>

#define rtc_address 0x70
#define rtc_data 0x71

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
      outb(rtc_address, 0x0A);
      return (inb(rtc_data) & update_in_progress_flag);
}
uint8 readCMOSRegister(uint8 reg) {
	outb(rtc_address, (!NMIsEnabled << 7) | (reg));
	return inb(rtc_data);
}

void RTC_handler(struct regs *r __attribute__((__unused__))) { // set to exactly 1024 Hz by default. We don't change the default
	outb(rtc_address, 0x0C);
	uint8 source = inb(rtc_data);

	if (source & 0x40) { // periodic interrupt
		RTC_Tick();
	} else if (source & 0x20) { // alarm interrupt
		// printf("RTC Alarm int\n");
	} else if (source & 0x10) { // Update-ended interrupt
		// printf("RTC Update-ended int\n");
	}

}

void RTC_install() {
    irq_install_handler(8, RTC_handler);

    outb(rtc_address, 0x8B);
    uint8 prev = inb(rtc_data);
    outb(rtc_address, 0x8B);
    outb(rtc_data, prev | 0x40);

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

	// printf("%i/%i/%i %i:%i:%i\n", month, day, year, hours, minutes, seconds);
	setTime(getTimeFromHuman(time));

}



