#include <io.h>
#include <interrupts/pic.h>

// Function for remapping the PICs (Programmable Interrupt Controllers)

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21

#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */



void PIC_remap() {
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

	outb(PIC1_DATA, 0x20); // Map PIC1 vectors to 0x20 - 0x27
    io_wait();
	outb(PIC2_DATA, 0x28); // Map PIC2 vectors to 0x28 - 0x47
    io_wait();

	outb(PIC1_DATA, 0x04); // Tell PIC1 that PIC2 is at IRQ2 (0000 0100)
    io_wait();
	outb(PIC2_DATA, 0x02); // Tell PIC2 that it's cascade identity is 2
    io_wait();

	outb(PIC1_DATA, ICW4_8086); // Set PIC1 to 8086 mode
    io_wait();
	outb(PIC2_DATA, ICW4_8086); // Set PIC2 to 8086 mode
    io_wait();

	outb(PIC1_DATA, 0x0); // Mask all interrupts
    io_wait();
	outb(PIC2_DATA, 0x0); // Mask all interrupts
    io_wait();
}
