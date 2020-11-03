#include <io.h>
#include <pit.h>
#include <system.h>
#include <time.h>


void tone(uint32 freq) {
	if (freq == 0) {
		return;
	}
	configurePIT(TIMER2_SELECT, TIMER2_CTL, freq, MODE_SQUARE_WAVE);
	
	outportb(0x61, inportb(0x61) | 3);
	
}
void noTone() {
	outportb(0x61, inportb(0x61) & ~3);
}
void toneDuration(uint32 freq, uint32 duration) {
	tone(freq);
	wait(duration);
	noTone();
}
