#include <system.h>

#pragma once

void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);

void gdt_install();
