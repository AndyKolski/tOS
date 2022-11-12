#include <stddef.h>
#include <string.h>
#include <system.h>

struct gdt_entry_bits {
	uint32 offset_low             : 16;
	uint32 selector               : 16;
	uint32 always0                :  8;
	uint32 gateType               :  4; // readable for code, writable for data
	uint32 storageSegment         :  1; // conforming for code, expand down for data
	uint32 DPL                    :  2; // 1 for code, 0 for data
	uint32 present                :  1; // should be 1 for everything but TSS and LDT
	uint32 offset_high            :  16; // privilege level
} __attribute__((packed));

struct idt_ptr
{
	unsigned short limit;
	unsigned int base;
} __attribute__((packed));


struct gdt_entry_bits idt[256] = {0};
struct idt_ptr idtp;


extern void idt_load();


void idt_set_gate(uint8 num, uint32 base, uint16 selector, uint8 gate, bool storageSegment, uint8 minRing) {
	idt[num].offset_low = (base & 0xFFFF);
	idt[num].selector = selector;
	idt[num].always0 = 0;
	idt[num].gateType = gate;
	idt[num].storageSegment = storageSegment;
	idt[num].DPL = minRing;
	idt[num].present = 1;
	idt[num].offset_high = (base >> 16) & 0xFFFF;
}



void idt_install()
{
	idtp.limit = (sizeof (struct gdt_entry_bits) * 256) - 1;
	idtp.base = (uint32) &idt;

	idt_load();
}