#ifndef __SYSTEM_H
#define __SYSTEM_H

#define false 0
#define true 1
#define bool int

typedef unsigned char           uint8;
typedef unsigned short          uint16;
typedef unsigned int            uint32;
typedef unsigned long long      uint64;

/* This defines what the stack looks like after an ISR was running */
struct regs
{
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};

/* MAIN.C */
extern char *memcpy(char *dest, const char *src, int count);
extern char *memset(char *dest, char val, int count);
extern short *memsetw(short *dest, short val, int count);
extern int strlen(const char *str);
extern int itoa(int value, char *sp, int radix);
extern unsigned char inportb (unsigned short _port);
extern void outportb (unsigned short _port, unsigned char _data);

/* SCRN.C */
extern int csr_x;
extern int csr_y;
extern int upd_csr_pos;
extern void cls();
extern void putch(char c);
extern void puts(char *str);
extern void putl(char *str);
extern void settextcolor(unsigned char forecolor, unsigned char backcolor);
extern void init_video();

/* GDT.C */
extern void gdt_install();

/* IDT.C */
extern void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
extern void idt_install();

/* ISRS.C */
extern void isrs_install();

/* IRQ.C */
extern void irq_install();
extern void irq_install_handler(int irq, void (*handler)(struct regs *r));
extern void irq_uninstall_handler(int irq);

/* TIMER.C */
extern void timer_install();
extern void timer_phase(int hz);
extern long timer_uptime_ticks();
extern long timer_uptime_secs();
extern void timer_wait_ms(unsigned int ms);
extern void timer_wait_ticks(unsigned int ticks);

/* KB.C */
extern void keyboard_install();
extern void kbd_led_handling(unsigned char ledstatus);

/* DISPLAY.C */

void install_display(uint64 fb_addr, uint32 fb_width, uint32 fb_height, uint8 fb_bpp, uint32 fb_pitch);
uint32 colorFromRGB(uint8 r, uint8 g, uint8 b);
void setPixel(uint32 x, uint32 y, uint32 c);
void fillRect(uint32 x, uint32 y, uint32 w, uint32 h, uint32 c);


#endif
