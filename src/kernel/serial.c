#include <display.h>
#include <io.h>
#include <interrupts/irq.h>
#include <keyboard.h>
#include <stdio.h>
#include <string.h>
#include <system.h>

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

uint16 getPortFromNumber(uint8 com) {
	switch(com) {
		case 1:
			return COM1;
		case 2:
			return COM2;
		case 3:
			return COM3;
		case 4:
			return COM4;
		default:
			panic("Invalid COM number");
			return 0;
	}
}

uint8 bitsPerCharFromNumber(uint8 number) {
	switch(number) {
		case 5:
			return 0b00;
		case 6:
			return 0b01;
		case 7:
			return 0b10;
		case 8:
			return 0b11;
		default:
			panic("Invalid number of bits per character");
			return 0;
	}
}

bool configurePort(uint8 com, uint32 baudRate, uint8 bitsPerChar, bool useTwoStopBits, bool useParity, bool evenParity, bool stickParity) {

	uint16 port = getPortFromNumber(com);

	uint8 bitsPerCharValue = bitsPerCharFromNumber(bitsPerChar);

	uint16 divisor = 115200 / baudRate;


	outb(port + 3, 0x00); // set DTAB to 0
	outb(port + 1, 0x00); // Interrupt Enable Register - Disable interrupts


	outb(port + 3, 0x80); // set DTAB to 1
	outb(port + 0, divisor & 0xff); // Baud rate divisor least significant byte
	outb(port + 1, divisor >> 8); // Baud rate divisor most significant byte

	outb(port + 2, 0b01000111); // FIFO control registers - Interrupt after 1 byte, clear everything

	uint8 portReg3Value = (bitsPerCharValue & 0x03) | (useTwoStopBits << 2) | (useParity << 3) | (evenParity << 4) | (stickParity << 5); // Line Control Register - bits 0-1 are bits per char, bit 2 is stop bits, 3 is parity enable, 4 is even parity, 5 is stick parity
	outb(port + 3, portReg3Value);

	outb(port + 4, 0x10); // Modem Control Register - we enable loopback to test the port

	outb(port + 0, 0xaa);

	if(inb(port + 0) != 0xaa) {
		  return false;
	}

	outb(port + 3, 0x00); // set DTAB to 0
	outb(port + 1, 0x01); // Interrupt Enable Register - Enable interrupts
	outb(port + 3, portReg3Value); // restore original settings after using to set DTAB
	outb(port + 4, 0x0B); // the port seems to work, we enable it

	return true;
}

volatile bool portsEnabled[4] = {0};
volatile uint8 usePort = 0;

void serial_handler(struct regs *r __attribute__((__unused__))) {
	for (int i = 0; i < 4; ++i) {
		if (!portsEnabled[i]) {
			continue;
		} else {
			uint16 port = getPortFromNumber(i+1);
			uint8 intStatus = inb(port + 2);
			if ((intStatus & 0x01) == 1) { // this port has no pending interrupts to handle
				continue;
			} else { // the current port has an interrupt we need to handle
				uint8 status = (intStatus & 0x06) >> 1;
				if (status == 0) { // Modem Status Interrupt

				} else if (status == 1) { // Transmitter Holding Register Empty Interrupt
					
				} else if (status == 2) { // Data received
					uint8 read = inb(port + 0);
					if (i+1 == usePort) {
						keyboardKeyPress((char)read);
					}
					// printf("Serial in: %c\n", read);
				} else if (status == 3) { // Error
					uint8 error = inb(port + 5);
					error = (error >> 1) % 0x0f;
					if (error & 0x01) {
						printf("Serial port %i had an error: Overrun Error\n", i+1);
					} else if ((error >> 1) & 0x01) {
						printf("Serial port %i had an error: Parity Error\n", i+1);
					} else if ((error >> 2) & 0x01) {
						printf("Serial port %i had an error: Framing Error\n", i+1);
					} else if ((error >> 3) & 0x01) {
						printf("Serial port %i had an error: Break Interrupt Error\n", i+1);
					}
				}
			}
		}
	}
}

void initSerial() {
	for (int i = 0; i < 4; ++i) {
		portsEnabled[i] = configurePort(i+1, 9600, 8, 0, 0, 0, 0); // Try to configure the port at: 9600 baud, 8 bits per char, one stop bit, and no parity
		if (portsEnabled[i] && !usePort) {
			usePort = i+1;
		}
	}
	if (usePort == 0) {
		puts("No serial port present or detected");
	} else {
		printf("Using serial port: %i\n", usePort);
	}
	irq_install_handler(4, serial_handler);
	irq_install_handler(3, serial_handler);
}

void serial_putc(char chr) {
	if (usePort == 0 || !portsEnabled[usePort-1]) {
		return;
	}
	uint16 port = getPortFromNumber(usePort);
	while (!(inb(port + 5) & 0x20)) {}
	outb(port, (uint8)chr);
}