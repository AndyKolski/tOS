#include <interrupts/irq.h>
#include <io.h>
#include <stdio.h>
#include <system.h>
#include <time/time.h>

#define RTC_REGISTER_SELECT_PORT 0x70
#define RTC_DATA_PORT            0x71

#define RTC_REGISTER_SECONDS  0x00
#define RTC_REGISTER_MINUTES  0x02
#define RTC_REGISTER_HOURS    0x04
#define RTC_REGISTER_WEEKDAY  0x06
#define RTC_REGISTER_DAY      0x07
#define RTC_REGISTER_MONTH    0x08
#define RTC_REGISTER_HALFYEAR 0x09
#define RTC_REGISTER_CENTURY  0x32
#define RTC_REGISTER_STATUS_A 0x0A
#define RTC_REGISTER_STATUS_B 0x0B
#define RTC_REGISTER_STATUS_C 0x0C

#define RTC_UPDATE_IN_PROGRESS_FLAG 0x80

uint8 bcdToDecimal(uint8 bcd) {
	assert(((bcd & 0xF0) >> 4) < 10, "invalid BCD conversion"); // More significant nibble is valid
	assert((bcd & 0x0F) < 10, "invalid BCD conversion");        // Less significant nibble is valid
	return ((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F);
}

bool disableNMIs = false;
uint8 readCMOSRegister(uint8 reg) {
	// PCs have a weird old hack here. When the 7th bit of the address port is 1, NMIs are disabled and aren't sent to the CPU
	outb(RTC_REGISTER_SELECT_PORT, (disableNMIs << 7) | (reg));
	io_wait();
	return inb(RTC_DATA_PORT);
}

/// @brief Checks if the RTC is part-way through updating the values in the registers (and so they may not be valid)
/// @return True if the RTC is updating, false otherwise
bool isRTCUpdating() {
	outb(RTC_REGISTER_SELECT_PORT, RTC_REGISTER_STATUS_A);
	return (inb(RTC_DATA_PORT) & RTC_UPDATE_IN_PROGRESS_FLAG);
}

void RTC_handler(struct regs *r __attribute__((__unused__))) { // set to exactly 1024 Hz by default. We don't change the default
	outb(RTC_REGISTER_SELECT_PORT, RTC_REGISTER_STATUS_C);
	uint8 source = inb(RTC_DATA_PORT);

	if (source & 0x40) { // periodic interrupt
		RTC_Tick();
	} else if (source & 0x20) { // alarm interrupt
		// printf("RTC Alarm int\n");
	} else if (source & 0x10) { // Update-ended interrupt
		// printf("RTC Update-ended int\n");
	}
}

HumanTime readRTC() {
	// Wait until the RTC is not updating the time before we read it
	while (isRTCUpdating()) {}

	uint8 statusRegisterBValue = readCMOSRegister(RTC_REGISTER_STATUS_B);

	bool twentyFourHourMode = statusRegisterBValue & 0x02;
	bool binaryMode = statusRegisterBValue & 0x04;

	uint8 seconds = readCMOSRegister(RTC_REGISTER_SECONDS);
	uint8 minutes = readCMOSRegister(RTC_REGISTER_MINUTES);
	uint8 hours = readCMOSRegister(RTC_REGISTER_HOURS);
	uint8 day = readCMOSRegister(RTC_REGISTER_DAY);
	uint8 month = readCMOSRegister(RTC_REGISTER_MONTH);
	uint8 halfYear = readCMOSRegister(RTC_REGISTER_HALFYEAR);
	uint8 century = readCMOSRegister(RTC_REGISTER_CENTURY);
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

	year = (uint32)(century * 100) + halfYear;

	// printf("%i/%i/%i %i:%i:%i\n", month, day, year, hours, minutes, seconds);

	HumanTime time = {0};

	time.seconds = seconds;
	time.minutes = minutes;
	time.hours = hours;
	time.day = day;
	time.month = month;
	time.year = year;

	return time;
}

void RTC_install() {
	irq_install_handler(8, RTC_handler);

	// Enable interrupts from the RTC
	outb(RTC_REGISTER_SELECT_PORT, 0x8B);
	uint8 prev = inb(RTC_DATA_PORT);
	outb(RTC_REGISTER_SELECT_PORT, 0x8B);
	outb(RTC_DATA_PORT, prev | 0x40);

	setTime(getTimeFromHuman(readRTC()));
}
