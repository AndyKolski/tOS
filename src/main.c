#include <header.h>
#include <multiboot.h>

char *memcpy(char *dest, const char *src, int count)
{
	for (int i = 0; i < count; ++i) {
		dest[i] = src[i];
	}
	return dest;
}

void memcpyint(void *dest, void *src, int n) {
	int *csrc = (int *) src;
	int *cdest = (int *) dest;

	for (int i=0; i<n; i++)
		cdest[i] = csrc[i];
}

char *memset(char *dest, char val, int count)
{
	for (int i = 0; i < count; ++i) {
		dest[i] = val;
	}
	return dest;
}

short *memsetw(short *dest, short val, int count)
{
	for (int i = 0; i < count; ++i) {
		dest[i] = val;
	}
	return dest;
}

int strlen(const char *str)
{
	int l = 0;
	while (str[l] != 0) {
		l++;
	}
	return l;
}
unsigned char inportb (unsigned short _port)
{
	unsigned char rv;
	__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}
void outportb (unsigned short _port, unsigned char _data)
{
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}
void reverse(char s[]) {
	 int i, j;
	 char c;
 
	 for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
		 c = s[i];
		 s[i] = s[j];
		 s[j] = c;
	 }
 }
 int itoa(int value, char *sp, int radix)
{
	char tmp[16];// be careful with the length of the buffer
	char *tp = tmp;
	int i;
	unsigned v;
	int sign = (radix == 10 && value < 0);    
	if (sign)
		v = -value;
	else
		v = (unsigned)value;

	while (v || tp == tmp)
	{
		i = v % radix;
		v /= radix; // v/=radix uses less CPU clocks than v=v/radix does
		if (i < 10)
		  *tp++ = i+'0';
		else
		  *tp++ = i + 'A' - 10;
	}

	int len = tp - tmp;

	if (sign) 
	{
		*sp++ = '-';
		len++;
	}

	while (tp > tmp)
		*sp++ = *--tp;

	return len;
}

int kmain(unsigned long magic, unsigned long addr) {
	multiboot_info_t *mbi;
	mbi = (multiboot_info_t *) addr;
	install_display(mbi->framebuffer_addr, mbi->framebuffer_width, mbi->framebuffer_height, mbi->framebuffer_bpp, mbi->framebuffer_pitch);
	char magicbuf[15];
	init_video();
	puts("Magic value: 0x");
	itoa(magic, magicbuf, 16);
	puts(magicbuf);
	puts("\n");
	putl("Installing GDT...");
	gdt_install();
	putl("Installing IDT...");
	idt_install();
	putl("Installing ISRs...");
	isrs_install();
	putl("Installing IRQs...");
	irq_install();
	putl("Setting up System Timer at 2048 Hz...");
	timer_phase(2048);
	timer_install();
	putl("Setting up Keyboard Controller...");
	keyboard_install();
	putl("Setting Interrupt flag...");
	__asm__ __volatile__ ("sti"); 
	putl("Done");
	putl("");

	uint8 r = 0xff;
	uint8 g = 0;
	uint8 b = 0;
	uint32 c = 0;

	uint8 buf[16];
	while (1) {
		/*if(r > 0 && b == 0) {
			r--;
			g++;
		}
		if(g > 0 && r == 0) {
			g--;
			b++;
		}
		if (b > 0 && g == 0) {
			r++;
			b--;
		}
		c = colorFromRGB(r,g,b);
		fillRect(mbi->framebuffer_width/4,mbi->framebuffer_height/4,mbi->framebuffer_width/2, mbi->framebuffer_height/2, c);	
		*/
		drawDuck();
		/*itoa(timer_uptime_ticks(), buf, 10);
		badPlaceStr("Testing 123. The quick brown fox jumped over the lazy dog. !@#$%^&*()_+1234567890-=", 50, 50, colorFromRGB(0xff, 0xff, 0xff));
		badPlaceStr("System uptime in ticks:", 50, 50+11, colorFromRGB(0xff, 0xff, 0xff));
		badPlaceStrBkg(buf, 50, 50+22, colorFromRGB(0xff, 0xff, 0xff), colorFromRGB(0x00, 0x00, 0x00));
		*/
	}

	for(;;);
	return 0;
}