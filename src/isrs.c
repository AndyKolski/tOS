#include <idt.h>
#include <isrs.h>
#include <stdio.h>
#include <system.h>

/* These are function prototypes for all of the exception
*  handlers=The first 32 entries in the IDT are reserved
*  by Intel, and are designed to service exceptions! */
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

/* This is a very repetitive function... it's not hard, it's
*  just annoying. As you can see, we set the first 32 entries
*  in the IDT to the first 32 ISRs. We can't use a for loop
*  for this, because there is no way to get the function names
*  that correspond to that given entry. We set the access
*  flags to 0x8E. This means that the entry is present, is
*  running in ring 0 (kernel level), and has the lower 5 bits
*  set to the required '14', which is represented by 'E' in
*  hex. */
void isrs_install() {
	idt_set_gate(0, (unsigned)isr0, 0x08, 0xE, false, 0);
	idt_set_gate(1, (unsigned)isr1, 0x08, 0xE, false, 0);
	idt_set_gate(2, (unsigned)isr2, 0x08, 0xE, false, 0);
	idt_set_gate(3, (unsigned)isr3, 0x08, 0xE, false, 0);
	idt_set_gate(4, (unsigned)isr4, 0x08, 0xE, false, 0);
	idt_set_gate(5, (unsigned)isr5, 0x08, 0xE, false, 0);
	idt_set_gate(6, (unsigned)isr6, 0x08, 0xE, false, 0);
	idt_set_gate(7, (unsigned)isr7, 0x08, 0xE, false, 0);
	idt_set_gate(8, (unsigned)isr8, 0x08, 0xE, false, 0);
	idt_set_gate(9, (unsigned)isr9, 0x08, 0xE, false, 0);
	idt_set_gate(10, (unsigned)isr10, 0x08, 0xE, false, 0);
	idt_set_gate(11, (unsigned)isr11, 0x08, 0xE, false, 0);
	idt_set_gate(12, (unsigned)isr12, 0x08, 0xE, false, 0);
	idt_set_gate(13, (unsigned)isr13, 0x08, 0xE, false, 0);
	idt_set_gate(14, (unsigned)isr14, 0x08, 0xE, false, 0);
	idt_set_gate(15, (unsigned)isr15, 0x08, 0xE, false, 0);
	idt_set_gate(16, (unsigned)isr16, 0x08, 0xE, false, 0);
	idt_set_gate(17, (unsigned)isr17, 0x08, 0xE, false, 0);
	idt_set_gate(18, (unsigned)isr18, 0x08, 0xE, false, 0);
	idt_set_gate(19, (unsigned)isr19, 0x08, 0xE, false, 0);
	idt_set_gate(20, (unsigned)isr20, 0x08, 0xE, false, 0);
	idt_set_gate(21, (unsigned)isr21, 0x08, 0xE, false, 0);
	idt_set_gate(22, (unsigned)isr22, 0x08, 0xE, false, 0);
	idt_set_gate(23, (unsigned)isr23, 0x08, 0xE, false, 0);
	idt_set_gate(24, (unsigned)isr24, 0x08, 0xE, false, 0);
	idt_set_gate(25, (unsigned)isr25, 0x08, 0xE, false, 0);
	idt_set_gate(26, (unsigned)isr26, 0x08, 0xE, false, 0);
	idt_set_gate(27, (unsigned)isr27, 0x08, 0xE, false, 0);
	idt_set_gate(28, (unsigned)isr28, 0x08, 0xE, false, 0);
	idt_set_gate(29, (unsigned)isr29, 0x08, 0xE, false, 0);
	idt_set_gate(30, (unsigned)isr30, 0x08, 0xE, false, 0);
	idt_set_gate(31, (unsigned)isr31, 0x08, 0xE, false, 0);
}

/* This is a simple string array. It contains the message that
*  corresponds to each and every exception. We get the correct
*  message by accessing like:
*  exception_message[interrupt_number] */
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
		printf("\n [!!!] Interrupt #%i - %s, ErrCode=0x%08x\n\
	EDI=0x%08x ESI=0x%08x EBP=0x%08x EBX=0x%08x EDX=0x%08x ECX=0x%08x EAX=0x%08x\n\
	EIP=0x%08x  CS=0x%08x EFLAGS=0x%08x\n", \
	r->int_no, exceptionMessages[r->int_no], r->err_code, r->edi, r->esi, r->ebp, r->ebx, r->edx, r->ecx, r->eax, r->eip, r->cs, r->eflags);

		if (r->int_no == E_PAGE_FAULT) {
			uint32 faultAddress;

			asm("mov %%cr2, %0" : "=r"(faultAddress));

			printf("    A %s process caused a protection fault while %s a %s page at address 0x%08lx during %s.\n", \
				r->err_code & 1<<2 ? "user" : "kernel", \
				r->err_code & 1<<1 ? "writing to" : "reading from", \
				r->err_code & 1<<0 ? "present" : "non-present", \
				faultAddress, \
				r->err_code & 1<<4 ? "instruction fetch" : "data access");

			if (r->err_code & 1<<3) {
				printf("	One or more reserved bits were set to 1.\n");
			}
			if (r->err_code & 1<<5) {
				printf("	A protection-key violation occurred.\n");
			}
			if (r->err_code & 1<<6) {
				printf("	A shadow-stack access fault occurred.\n");
			}
			if (r->err_code & 1<<15) {
				printf("	An SGX violation occurred.\n");
			}

		}

		halt();
	}

}