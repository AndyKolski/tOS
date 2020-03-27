#include <system.h>
#ifndef SERIAL_H
# define SERIAL_H

void init_serial();
void serial_puts(char *text);
void serial_putc(char chr);

#endif