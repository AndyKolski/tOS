#include <system.h>

struct gdt_entry_bits {
    uint32 limit_low              : 16;
    uint32 base_low               : 24;
    uint32 accessed               :  1;
    uint32 read_write             :  1; // readable for code, writable for data
    uint32 conforming_expand_down :  1; // conforming for code, expand down for data
    uint32 executable             :  1; // 1 for code, 0 for data
    uint32 not_TSS_or_LDT         :  1; // should be 1 for everything but TSS and LDT
    uint32 ring                   :  2; // privilege level
    uint32 present                :  1;
    uint32 limit_high             :  4;
    uint32 available              :  1; // only used in software; has no effect on hardware
    uint32 long_mode              :  1;
    uint32 big                    :  1; // 32-bit opcodes for code, uint32 stack for data
    uint32 gran                   :  1; // 1 to use 4k page addressing, 0 for byte addressing
    uint32 base_high              :  8;
} __attribute__((packed));

struct tss_entry_struct {
    uint32 prev_tss; // The previous TSS
    uint32 esp0;     // The stack pointer to load when changing to kernel mode.
    uint32 ss0;      // The stack segment to load when changing to kernel mode.
    // Everything below here is unused.
    uint32 esp1;
    uint32 ss1;
    uint32 esp2;
    uint32 ss2;
    uint32 cr3;
    uint32 eip;
    uint32 eflags;
    uint32 eax;
    uint32 ecx;
    uint32 edx;
    uint32 ebx;
    uint32 esp;
    uint32 ebp;
    uint32 esi;
    uint32 edi;
    uint32 es;
    uint32 cs;
    uint32 ss;
    uint32 ds;
    uint32 fs;
    uint32 gs;
    uint32 ldt;
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed));
typedef struct tss_entry_struct tss_entry_t;

tss_entry_t tss_entry = {0};

struct gdt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

struct gdt_entry_bits gdt[6] = {0};
struct gdt_ptr gp;

extern void gdt_flush();
extern void flush_tss();

void gdt_set_gate(int number, uint32 base, uint32 limit, bool present, uint8 ring, bool not_TSS_or_LDT, bool executable, bool direction_or_conforming, bool RW, bool is32Bit) {
    gdt[number].limit_low = (limit/4096) & 0xFFFF;
    gdt[number].limit_high = ((limit/4096)>>16) & 0xF;
    gdt[number].base_low = base & 0xffffff00;
    gdt[number].base_high = (base >> 24) & 0xff;

    gdt[number].accessed = 0;
    gdt[number].read_write = RW; // since this is a code segment, specifies that the segment is readable
    gdt[number].conforming_expand_down = direction_or_conforming; // does not matter for ring 3 as no lower privilege level exists
    gdt[number].executable = executable;
    gdt[number].not_TSS_or_LDT = not_TSS_or_LDT;
    gdt[number].ring = ring; // ring 3
    gdt[number].present = present;
    gdt[number].available = 1;
    gdt[number].long_mode = 0;
    gdt[number].big = is32Bit; // it's 32 bits
    gdt[number].gran = 1; // 4KB page addressing
}
void set_tss(int gdt_number) {

    uint32 tssBase = (uint32) &tss_entry;
    uint32 tssLimit = sizeof tss_entry;

    gdt[gdt_number].limit_low = tssLimit;
    gdt[gdt_number].limit_high = (tssLimit>>16) & 0xF;

    gdt[gdt_number].base_low = tssBase;
    gdt[gdt_number].base_high = (tssBase >> 24) & 0xff; //isolate top byte

    gdt[gdt_number].accessed = 1; // With a system entry (`code_data_segment` = 0), 1 indicates TSS and 0 indicates LDT
    gdt[gdt_number].read_write = 0; // For a TSS, indicates busy (1) or not busy (0).
    gdt[gdt_number].conforming_expand_down = 0; // always 0 for TSS
    gdt[gdt_number].executable = 1; // For a TSS, 1 indicates 32-bit (1) or 16-bit (0).
    gdt[gdt_number].not_TSS_or_LDT=0; // indicates TSS/LDT (see also `accessed`)
    gdt[gdt_number].ring = 3; // user mode
    gdt[gdt_number].present = 1;
    gdt[gdt_number].available = 0; // 0 for a TSS
    gdt[gdt_number].long_mode = 0;
    gdt[gdt_number].big = 0; // should leave zero according to manuals.
    gdt[gdt_number].gran = 0; // limit is in bytes, not pages
    
    uint32 esp = 0;
    asm("mov %%esp, %0" : "=a"(esp));

    tss_entry.ss0 = 0x10;
    tss_entry.esp0 = esp;
}

void gdt_install() {
    gp.limit = (sizeof(struct gdt_entry_bits) * (sizeof(gdt)/sizeof(struct gdt_entry_bits))) - 1;
    gp.base = (uint32) &gdt;

    // the NULL descriptor does not need to be explicitly set, as the GDT is initialized to zeros

   // kernel code segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, true, 0, true, true, 0, true, true);

    // kernel data segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, true, 0, true, false, 0, true, true);

    // ring 3 code segment
    gdt_set_gate(3, 0, 0xFFFFFFFF, true, 3, true, true, 0, true, true);

    // ring 3 data segment
    gdt_set_gate(4, 0, 0xFFFFFFFF, true, 3, true, false, 0, true, true);

    set_tss(5);

    gdt_flush();

    flush_tss();

}

void set_kernel_stack(uint32 stack) { // Used when an interrupt occurs
    tss_entry.esp0 = stack;
}