#include <system.h>

#pragma once

void timer_install();
void timer_phase(int hz);
long timer_uptime_ticks();
long timer_uptime_secs();
void timer_wait_ms(unsigned int ms);
void timer_wait_ticks(unsigned int ticks);