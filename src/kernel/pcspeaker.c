#include <io.h>
#include <system.h>
#include <time/pit.h>
#include <time/time.h>


void tone(uint32 freq) {
	if (freq == 0) {
		return;
	}
	configurePIT(TIMER2_SELECT, TIMER2_CTL, freq, MODE_SQUARE_WAVE);
	
	outb(0x61, inb(0x61) | 3);
	
}
void noTone() {
	outb(0x61, inb(0x61) & ~3);
}
void toneDuration(uint32 freq, uint32 duration) {
	tone(freq);
	wait(duration);
	noTone();
}
