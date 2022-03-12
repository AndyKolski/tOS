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
	outb(PIT_CTL, timerSelect | WRITE_WORD | mode);

	outb(timerCTL, (uint8) (Div));
	outb(timerCTL, (uint8) (Div >> 8));	
}

void PIT_handler(struct regs *r __attribute__((__unused__))) {
	PIT_Tick();
}

void PIT_Install(uint32 freq) {
	setPITRate(freq);
	irq_install_handler(0, PIT_handler);
}

void setPITRate(uint32 freq) {
	printf("Setting PIT to %lu Hz...\n", freq);
	configurePIT(TIMER0_SELECT, TIMER0_CTL, freq, MODE_SQUARE_WAVE);
}