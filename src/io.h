#include <system.h>
#ifndef IO_H
# define IO_H

uint8 inportb (uint16 _port);

void outportb (uint16 _port, uint8 _data);

uint8_t inb(uint16 port);

void outb(uint16 port, uint8 val);

#endif