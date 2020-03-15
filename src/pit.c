#include <header.h>


#define PIT_CTL 0x43


#define WRITE_WORD 0x30


void configureTimer(uint8 timerSelect, uint8 timerCTL, uint32 freq, uint8 mode) {
	uint32 Div = 1193182 / freq;
	outportb(PIT_CTL, timerSelect | WRITE_WORD | mode);

	outportb(timerCTL, (uint8) (Div));
	outportb(timerCTL, (uint8) (Div >> 8));

	
}