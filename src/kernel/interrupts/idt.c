#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <system.h>

typedef struct interruptDescriptor {
	uint16 lowAddress;
	uint16 codeSegment;
	uint8 IST;
	uint8 gateType;
	uint16 middleAddress;
	uint32 highAddress;
	uint32 reserved;
} __attribute__((packed)) interruptDescriptor;

COMPILE_TIME_ASSERT(sizeof(interruptDescriptor) == 16, IDT_ENTRY_MUST_BE_16_BYTES);

typedef struct idt_ptr {
	uint16 size;
	uint64 address;
} __attribute__((packed)) idt_ptr;

COMPILE_TIME_ASSERT(sizeof(idt_ptr) == 10, IDT_POINTER_STRUCT_MUST_BE_10_BYTES);

__attribute__((aligned(0x10))) static interruptDescriptor idt[256] = {0};
idt_ptr idt_pointer = {0};

void idt_set_gate(uint8 index, void (*handler)(void), uint8 gate, uint8 minRing) {
	uint64 handler_address = (uint64)handler;
	idt[index].lowAddress = (handler_address & 0xFFFF);
	idt[index].middleAddress = (handler_address >> 16) & 0xFFFF;
	idt[index].highAddress = (handler_address >> 32) & 0xFFFFFFFF;

	idt[index].codeSegment = gdt_kernel_code_segment;
	idt[index].IST = 0b111 & 0;
	idt[index].gateType = (gate & 0b1111) | ((minRing & 0b11) << 5) | 0b10000000;

	idt[index].reserved = 0;
}

void initIDT() {
	PIC_remap();

	idt_pointer.size = sizeof(interruptDescriptor) * 256 - 1;
	idt_pointer.address = (uint64)&idt;

	asm volatile("lidt %0" : : "m"(idt_pointer));
}
