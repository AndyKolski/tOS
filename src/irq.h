#include <system.h>
#ifndef IRQ_H
# define IRQ_H

void irq_install();
void irq_install_handler(int irq, void (*handler)(struct regs *r));
void irq_uninstall_handler(int irq);

#endif