#include <system.h>
#ifndef TIMER_H
# define TIMER_H

void timer_install();
void timer_phase(int hz);
long timer_uptime_ticks();
long timer_uptime_secs();
void timer_wait_ms(unsigned int ms);
void timer_wait_ticks(unsigned int ticks);

#endif