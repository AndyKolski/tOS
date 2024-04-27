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
	idt_set_gate(32 + 0, (uintptr_t)irq0, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 1, (uintptr_t)irq1, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 2, (uintptr_t)irq2, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 3, (uintptr_t)irq3, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 4, (uintptr_t)irq4, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 5, (uintptr_t)irq5, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 6, (uintptr_t)irq6, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 7, (uintptr_t)irq7, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 8, (uintptr_t)irq8, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 9, (uintptr_t)irq9, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 10, (uintptr_t)irq10, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 11, (uintptr_t)irq11, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 12, (uintptr_t)irq12, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 13, (uintptr_t)irq13, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 14, (uintptr_t)irq14, GATE_INTERRUPT, 0);
	idt_set_gate(32 + 15, (uintptr_t)irq15, GATE_INTERRUPT, 0);
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
