#include <system.h>

#pragma once

uint8_t inb(uint16 port);

void outb(uint16 port, uint8 val);

void io_wait();