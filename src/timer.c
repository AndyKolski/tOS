#include <display.h>
#include <irq.h>
#include <kb.h>
#include <pit.h>
#include <system.h>
#include <timer.h>

/* This will keep track of how many ticks that the system
*  has been running for */
volatile uint64 timer_ticks = 0;
volatile uint32 timer_tps = 0;

/* Handles the timer. In this case, it's very simple: We
*  increment the 'timer_ticks' variable every time the
*  timer fires. By default, the timer fires 18.222 times
*  per second. Why 18.222Hz? Some engineer at IBM must've
*  been smoking something funky */
void timer_handler(struct regs *r __attribute__((__unused__)))
{
    /* Increment our 'tick count' */
    timer_ticks++;

    if (timer_ticks % timer_tps == 0)
    {
       setKeyboardLEDs(1 << (timer_ticks/timer_tps%3));
       // printf("System uptime: %i secs\n", timer_uptime_secs());
    }
}

long timer_uptime_ticks() {
	return timer_ticks;
}

long timer_uptime_secs() {
	return timer_ticks / timer_tps;
}

/* Sets up the system clock by installing the timer handler
*  into IRQ0 */
void timer_install()
{
    /* Installs 'timer_handler' to IRQ0 */
    irq_install_handler(0, timer_handler);
}

void timer_phase(int freq) {
    printf("Setting up System Timer at %i Hz...\n", freq);
    timer_tps = freq;
    configureTimer(TIMER0_SELECT, TIMER0_CTL, freq, MODE_SQUARE_WAVE);
}

void timer_wait_ms(unsigned int ms) {
    unsigned long eticks;

    eticks = timer_ticks + ((ms*timer_tps)/1000);
    while(timer_ticks < eticks) {

    }
}

void timer_wait_ticks(unsigned int ticks)
{
    unsigned long eticks;

    eticks = timer_ticks + ticks;
    while(timer_ticks < eticks) {

    }
}