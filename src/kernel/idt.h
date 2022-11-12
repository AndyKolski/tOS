#include <system.h>

#pragma once

void idt_set_gate(uint8 num, uint32 base, uint16 selector, uint8 gate, bool storageSegment, uint8 minRing);
void idt_install();
