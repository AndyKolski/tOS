#include <io.h>
#include <irq.h>
#include <pit.h>
#include <stdio.h>
#include <system.h>
#include <time.h>

#define PIT_CTL 0x43


#define WRITE_WORD 0x30


void configurePIT(uint8 timerSelect, uint8 timerCTL, uint32 freq, uint8 mode) {
	uint32 Div = 1193182 / freq;
	outportb(PIT_CTL, timerSelect | WRITE_WORD | mode);

	outportb(timerCTL, (uint8) (Div));
	outportb(timerCTL, (uint8) (Div >> 8));	
}

void PIT_Install() {
    irq_install_handler(0, timeTick);
}

void setPITRate(int freq) {
    printf("Setting up PIT at %i Hz...\n", freq);
    configurePIT(TIMER0_SELECT, TIMER0_CTL, freq, MODE_SQUARE_WAVE);
}