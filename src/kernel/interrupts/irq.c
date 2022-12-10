#include <interrupts/idt.h>
#include <io.h>
#include <stdio.h>
#include <system.h>

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void *irq_routines[16] = {0};

void irq_install_handler(int irq, void (*handler)(struct regs *r)) {
	irq_routines[irq] = handler;
}

void initIRQs() {
	idt_set_gate(32, (uintptr_t)irq0, GATE_INTERRUPT, 0);
	idt_set_gate(33, (uintptr_t)irq1, GATE_INTERRUPT, 0);
	idt_set_gate(34, (uintptr_t)irq2, GATE_INTERRUPT, 0);
	idt_set_gate(35, (uintptr_t)irq3, GATE_INTERRUPT, 0);
	idt_set_gate(36, (uintptr_t)irq4, GATE_INTERRUPT, 0);
	idt_set_gate(37, (uintptr_t)irq5, GATE_INTERRUPT, 0);
	idt_set_gate(38, (uintptr_t)irq6, GATE_INTERRUPT, 0);
	idt_set_gate(39, (uintptr_t)irq7, GATE_INTERRUPT, 0);
	idt_set_gate(40, (uintptr_t)irq8, GATE_INTERRUPT, 0);
	idt_set_gate(41, (uintptr_t)irq9, GATE_INTERRUPT, 0);
	idt_set_gate(42, (uintptr_t)irq10, GATE_INTERRUPT, 0);
	idt_set_gate(43, (uintptr_t)irq11, GATE_INTERRUPT, 0);
	idt_set_gate(44, (uintptr_t)irq12, GATE_INTERRUPT, 0);
	idt_set_gate(45, (uintptr_t)irq13, GATE_INTERRUPT, 0);
	idt_set_gate(46, (uintptr_t)irq14, GATE_INTERRUPT, 0);
	idt_set_gate(47, (uintptr_t)irq15, GATE_INTERRUPT, 0);
}

void irq_handler(struct regs *r) {
	void (*handler)(struct regs *r);

	handler = irq_routines[r->int_no - 32];
	if (handler) {
		handler(r);
	} else {
		printf("\n [!!!] Unhandled IRQ: %lu\n", r->int_no - 32);
	}


	// send an EOI (end of interrupt) command to the PICs.
	if (r->int_no >= 40)
	{
		outb(0xA0, 0x20);
	}
	outb(0x20, 0x20);
}
	