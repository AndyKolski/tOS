#include <interrupts/idt.h>
#include <interrupts/isrs.h>
#include <interrupts/pic.h>
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
	assert(irq >= 0 && irq < 16, "Invalid IRQ number");
	irq_routines[irq] = handler;
}

void initIRQs() {
	idt_set_gate(32 + 0, irq0, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 1, irq1, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 2, irq2, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 3, irq3, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 4, irq4, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 5, irq5, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 6, irq6, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 7, irq7, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 8, irq8, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 9, irq9, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 10, irq10, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 11, irq11, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 12, irq12, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 13, irq13, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 14, irq14, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 15, irq15, GATE_INTERRUPT, 0);
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
	if (r->int_no - 32 >= 8) {
		outb(PIC2_COMMAND_PORT, PIC_COMMAND_EOI);
	}
	outb(PIC1_COMMAND_PORT, PIC_COMMAND_EOI);
}
