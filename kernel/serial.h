#include <system.h>

#pragma once

void init_serial();
void serial_putc(char chr);
bool configurePort(uint8 com, uint32 baudRate, uint8 bitsPerChar, bool useTwoStopBits, bool useParity, bool evenParity, bool stickParity);