#include <system.h>

#pragma once

#define GATE_INTERRUPT 0xE
#define GATE_TRAP 0xF

void idt_set_gate(uint8 index, void (*handler)(void), uint8 gate, uint8 minRing);
void initIDT();
