#include <interrupts/pic.h>
#include <io.h>

// Function for remapping the PICs (Programmable Interrupt Controllers)

void PIC_remap() {
	outb(PIC1_COMMAND_PORT, PIC_ICW1_INIT | PIC_ICW1_ICW4);
	io_wait();
	outb(PIC2_COMMAND_PORT, PIC_ICW1_INIT | PIC_ICW1_ICW4);
	io_wait();

	outb(PIC1_DATA_PORT, 0x20); // Map PIC1 vectors to 0x20 - 0x27
	io_wait();
	outb(PIC2_DATA_PORT, 0x28); // Map PIC2 vectors to 0x28 - 0x47
	io_wait();

	outb(PIC1_DATA_PORT, 0x04); // Tell PIC1 that PIC2 is at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA_PORT, 0x02); // Tell PIC2 that it's cascade identity is 2
	io_wait();

	outb(PIC1_DATA_PORT, PIC_ICW4_8086); // Set PIC1 to 8086 mode
	io_wait();
	outb(PIC2_DATA_PORT, PIC_ICW4_8086); // Set PIC2 to 8086 mode
	io_wait();

	outb(PIC1_DATA_PORT, 0x0); // Mask all interrupts
	io_wait();
	outb(PIC2_DATA_PORT, 0x0); // Mask all interrupts
	io_wait();
}
