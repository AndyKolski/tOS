#include <system.h>
#ifndef PCSPEAKER_H
# define PCSPEAKER_H

void tone(uint32 freq);
void noTone();
void toneDuration(uint32 freq, uint32 duration);

#endif