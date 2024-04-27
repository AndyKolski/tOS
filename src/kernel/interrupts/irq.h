#include <interrupts/isrs.h>
#include <system.h>

#pragma once

void initIRQs();
void irq_install_handler(int irq, void (*handler)(struct regs *r));
