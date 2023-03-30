#include <interrupts/idt.h>
#include <interrupts/isrs.h>
#include <stdio.h>
#include <system.h>

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

void initISRs() {
	idt_set_gate(0, (uintptr_t)isr0, GATE_INTERRUPT, 0);
	idt_set_gate(1, (uintptr_t)isr1, GATE_INTERRUPT, 0);
	idt_set_gate(2, (uintptr_t)isr2, GATE_INTERRUPT, 0);
	idt_set_gate(3, (uintptr_t)isr3, GATE_INTERRUPT, 0);
	idt_set_gate(4, (uintptr_t)isr4, GATE_INTERRUPT, 0);
	idt_set_gate(5, (uintptr_t)isr5, GATE_INTERRUPT, 0);
	idt_set_gate(6, (uintptr_t)isr6, GATE_INTERRUPT, 0);
	idt_set_gate(7, (uintptr_t)isr7, GATE_INTERRUPT, 0);
	idt_set_gate(8, (uintptr_t)isr8, GATE_INTERRUPT, 0);
	idt_set_gate(9, (uintptr_t)isr9, GATE_INTERRUPT, 0);
	idt_set_gate(10, (uintptr_t)isr10, GATE_INTERRUPT, 0);
	idt_set_gate(11, (uintptr_t)isr11, GATE_INTERRUPT, 0);
	idt_set_gate(12, (uintptr_t)isr12, GATE_INTERRUPT, 0);
	idt_set_gate(13, (uintptr_t)isr13, GATE_INTERRUPT, 0);
	idt_set_gate(14, (uintptr_t)isr14, GATE_INTERRUPT, 0);
	idt_set_gate(15, (uintptr_t)isr15, GATE_INTERRUPT, 0);
	idt_set_gate(16, (uintptr_t)isr16, GATE_INTERRUPT, 0);
	idt_set_gate(17, (uintptr_t)isr17, GATE_INTERRUPT, 0);
	idt_set_gate(18, (uintptr_t)isr18, GATE_INTERRUPT, 0);
	idt_set_gate(19, (uintptr_t)isr19, GATE_INTERRUPT, 0);
	idt_set_gate(20, (uintptr_t)isr20, GATE_INTERRUPT, 0);
	idt_set_gate(21, (uintptr_t)isr21, GATE_INTERRUPT, 0);
	idt_set_gate(22, (uintptr_t)isr22, GATE_INTERRUPT, 0);
	idt_set_gate(23, (uintptr_t)isr23, GATE_INTERRUPT, 0);
	idt_set_gate(24, (uintptr_t)isr24, GATE_INTERRUPT, 0);
	idt_set_gate(25, (uintptr_t)isr25, GATE_INTERRUPT, 0);
	idt_set_gate(26, (uintptr_t)isr26, GATE_INTERRUPT, 0);
	idt_set_gate(27, (uintptr_t)isr27, GATE_INTERRUPT, 0);
	idt_set_gate(28, (uintptr_t)isr28, GATE_INTERRUPT, 0);
	idt_set_gate(29, (uintptr_t)isr29, GATE_INTERRUPT, 0);
	idt_set_gate(30, (uintptr_t)isr30, GATE_INTERRUPT, 0);
	idt_set_gate(31, (uintptr_t)isr31, GATE_INTERRUPT, 0);
}

char *exceptionMessages[] = {
	"Division by Zero Exception",
	"Debug Exception",
	"Non Maskable Interrupt Exception",
	"Breakpoint Exception",
	"Into Detected Overflow Exception",
	"Out of Bounds Exception",
	"Invalid Opcode Exception",
	"No Coprocessor Exception",
	"Double Fault Exception",
	"Coprocessor Segment Overrun Exception",
	"Bad TSS Exception",
	"Segment Not Present Exception",
	"Stack Fault Exception",
	"General Protection Fault Exception",
	"Page Fault Exception",
	"Unknown Interrupt Exception",
	"Coprocessor Fault Exception",
	"Alignment Check Exception",
	"Machine Check Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception",
	"Reserved Exception"
};
void fault_handler(struct regs *r) {
	if (r->int_no < 32) {
		printf("\n [!!!] Unhandled %s (#%lu) - Error code: 0x%08lx\n", exceptionMessages[r->int_no], r->int_no, r->err_code);

		printf("Register States:\n");
		printf(" RAX: 0x%016lx RBX: 0x%016lx RCX: 0x%016lx RDX: 0x%016lx\n", r->rax, r->rbx, r->rcx, r->rdx);
		printf(" RSI: 0x%016lx RDI: 0x%016lx RBP: 0x%016lx RSP: 0x%016lx\n", r->rsi, r->rdi, r->rbp, r->rsp);
		printf(" R8:  0x%016lx R9:  0x%016lx R10: 0x%016lx R11: 0x%016lx\n", r->r8,  r->r9,  r->r10, r->r11);
		printf(" R12: 0x%016lx R13: 0x%016lx R14: 0x%016lx R15: 0x%016lx\n", r->r12, r->r13, r->r14, r->r15);
		printf(" RIP: 0x%016lx CS:  0x%04lx SS:  0x%04lx RFL: 0x%08lx\n", r->rip, r->cs, r->ss, r->rflags);


		printf("Extra info:\n");

		if (r->int_no == E_PAGE_FAULT) {
			uint64 faultAddress;

			asm("movq %%cr2, %0" : "=r"(faultAddress));

			printf(" A %s process caused a protection fault while %s a %s page at address 0x%08lx.\n", \
				r->err_code & 1<<2 ? "user" : "kernel", \
				r->err_code & 1<<1 ? "writing to" : "reading from", \
				r->err_code & 1<<0 ? "present" : "non-present", \
				faultAddress);
			if (r->err_code & 1<<3) {
				printf(" One or more reserved bits were set to 1.\n");
			}
			if (r->err_code & 1<<4) {
				printf(" The fault occurred during instruction fetch.\n");
			}
			if (r->err_code & 1<<5) {
				printf(" A protection-key violation occurred.\n");
			}
			if (r->err_code & 1<<6) {
				printf(" A shadow-stack access fault occurred.\n");
			}
			if (r->err_code & 1<<15) {
				printf(" An SGX violation occurred.\n");
			}

		} else if (r->int_no == E_GENERAL_PROTECTION_FAULT && r->err_code != 0) {
			if (r->err_code & 1<<0) {
				printf(" The fault originated externally to the processor.\n");
			}

			char* faultTable;
			switch ((uint32)(r->err_code >> 1) & 0b11) {
				case 0b00:
					faultTable = "GDT";
					break;
				case 0b01:
					faultTable = "IDT";
					break;
				case 0b10:
					faultTable = "LDT";
					break;
				case 0b11:
					faultTable = "IDT";
					break;

				default: 
					// GCC doesn't realize that the switch statement covers all possible values and insists
					// we either do this or disable the relevant warning for this section.
					assertf("This line should be unreachable!");
					faultTable = "unknown descriptor table";
					break;
			}

			printf(" The fault was related to descriptor number 0x%lx in the %s.\n", (r->err_code >> 4) & 0x1fff, faultTable);
		}

		printf("\n [!!!] System halted.");
		halt();
	}

}
