#include <stdint.h>
#ifndef __SYSTEM_H
#define __SYSTEM_H

#define false 0
#define true 1
#define bool int

typedef unsigned char           uint8;
typedef unsigned short          uint16;
typedef unsigned int            uint32;
typedef unsigned long long      uint64;
typedef signed char           int8;
typedef signed short          int16;
typedef signed int            int32;
typedef signed long long      int64;

typedef char kchar;

/* Timer related ports */
#define TIMER0_CTL 0x40
#define TIMER1_CTL 0x41
#define TIMER2_CTL 0x42

/* Building blocks for PIT_CTL */
#define TIMER0_SELECT 0x00
#define TIMER1_SELECT 0x40
#define TIMER2_SELECT 0x80

#define MODE_COUNTDOWN 0x00
#define MODE_ONESHOT 0x02
#define MODE_RATE 0x04
#define MODE_SQUARE_WAVE 0x06


/* This defines what the stack looks like after an ISR was running */
struct regs
{
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};

int kmain(unsigned long magic, unsigned long addr);

/* COMMON.C */
uint8 *memcpy(uint8 *dest, const uint8 *src, uint32 count);
char *memset(char *dest, char val, int count);
short *memsetw(short *dest, short val, int count);
int strlen(const kchar *str);
char* strcpy(kchar* destination, const kchar* source);
int itoa(int64 value, char *sp, int radix);
unsigned char inportb (unsigned short _port);
void outportb (unsigned short _port, unsigned char _data);
void halt();
void reboot();

/* GDT.C */
void gdt_install();

/* IDT.C */
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
void idt_install();

/* ISRS.C */
void isrs_install();

/* IRQ.C */
void irq_install();
void irq_install_handler(int irq, void (*handler)(struct regs *r));
void irq_uninstall_handler(int irq);

/* TIMER.C */
void timer_install();
void timer_phase(int hz);
long timer_uptime_ticks();
long timer_uptime_secs();
void timer_wait_ms(unsigned int ms);
void timer_wait_ticks(unsigned int ticks);

/* KB.C */
void keyboard_install();
void kbd_led_handling(unsigned char ledstatus);

/* DISPLAY.C */
void install_display(uint64 fb_addr, uint32 fb_width, uint32 fb_height, uint8 fb_bpp, uint32 fb_pitch, bool useLegacy);
uint32 colorFromRGB(uint8 r, uint8 g, uint8 b);
void setPixel(uint32 x, uint32 y, uint32 c);
void fillRect(uint32 x, uint32 y, uint32 w, uint32 h, uint32 c);
void puts(kchar *text);
void putc(kchar chr);
void printf(kchar *pcFormat, ...);
void termBackspace(); //TEMP
uint32 framebuffer_width;
uint32 framebuffer_height;

/* MEMORY.C */
void install_memory(uint32 mmap_addr, uint32 mmap_length, uint32 *kmain);

/* PIT.C */
void configureTimer(uint8 timerSelect, uint8 timerCTL, uint32 freq, uint8 mode);

/* PCSPEAKER.C */
void tone(uint32 freq);
void noTone();
void toneDuration(uint32 freq, uint32 duration);

/* SERIAL.C */
void init_serial();
void serial_puts(char *text);
void serial_putc(char chr);
#endif
