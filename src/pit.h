#include <system.h>

#pragma once

/* Timer related ports */
#define TIMER0_CTL 0x40
#define TIMER1_CTL 0x41
#define TIMER2_CTL 0x42

/* Building blocks for PIT_CTL */
#define TIMER0_SELECT 0x00
#define TIMER1_SELECT 0x40
#define TIMER2_SELECT 0x80

#define MODE_COUNTDOWN 0x00
#define MODE_ONESHOT 0x02
#define MODE_RATE 0x04
#define MODE_SQUARE_WAVE 0x06

void configurePIT(uint8 timerSelect, uint8 timerCTL, uint32 freq, uint8 mode);

void PIT_Install();

void setPITRate(int freq);