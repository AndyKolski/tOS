#include <display.h>
#include <font.h>
#include <io.h>
#include <libs.h>
#include <serial.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <system.h>

 // TODO: The VESA framebuffer needs to be double-buffered, and it needs a 
 // cursor, among other fixes.

uint32 *ptr; // pointer to the graphical framebuffer
uint16 *basicPtr; // pointer to the text console

uint32 framebuffer_width = 0; // framebuffer width, in pixels
uint32 framebuffer_height = 0; // framebuffer height, in pixels
uint8 framebuffer_bpp = 0; // the number of bytes per pixel
uint32 framebuffer_pitch = 0; // the number of bytes between one pixel and the one directly below it

uint32 terminalWidth = 0; // the width of the terminal, in characters
uint32 terminalHeight = 0; // the height of the terminal, in characters

uint32 cursorx = 0; // the horizontal position of the cursor on the terminal, in characters
uint32 cursory = 0; // the vertical position of the cursor on the terminal, in characters

bool FBScreen = false; // is the display graphical or a text console

color_t textColor = GColWHITE;
color_t backgroundColor = GColBLACK;

void install_display(uint64 fb_addr, uint32 fb_width, uint32 fb_height, uint8 fb_bpp, uint32 fb_pitch, bool useLegacy) {
	FBScreen = !useLegacy;
	if (FBScreen) {
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
		ptr = (uint32 *)(uint64*)fb_addr; // this line causes a GCC warning, but AFAIK it cannot be fixed, so we disable it
		#pragma GCC diagnostic pop
		
		framebuffer_width = fb_width;
		framebuffer_height = fb_height;
		framebuffer_bpp = fb_bpp/8/4;
		framebuffer_pitch = fb_pitch/4;

		terminalWidth = framebuffer_width/(fontWidth+1);
		terminalHeight = framebuffer_height/(fontHeight+1);

		fillScreen(backgroundColor);
		printf("Created console with size %lux%lu (%lupx x %lupx @ %ibpp) at 0x%lx\n", terminalWidth, terminalHeight, framebuffer_width, framebuffer_height, fb_bpp, (uint32)fb_addr);
	} else {
		basicPtr = (uint16*) 0x000b8000;
		terminalWidth = 80;
		terminalHeight = 25;
		for (uint8 y = 0; y < 25; y++) {
			for (uint8 x = 0; x < 80; x++) {
				basicPtr[y * 80 + x] = ' ' | (15 | 0 << 4) << 8;
			}
		}
		printf("Created console with size %lux%lu (--px x --px @ --bpp)\n", terminalWidth, terminalHeight);
	}}

color_t colorFromRGB(uint8 r, uint8 g, uint8 b) {
	return (color_t)(b|(g<<8)|(r<<16));
}

color_t _colorFromHSV(uint8 h, uint8 s, uint8 v) {
	    uint8 r = 0;
	    uint8 g = 0;
	    uint8 b = 0;

	    uint8 region = 0;
	    uint8 remainder = 0;
	    uint8 p = 0;
	    uint8 q = 0;
	    uint8 t = 0;

	    if (s == 0) {
	        r = v;
	        g = v;
	        b = v;
	        return colorFromRGB(r, g, b);
	    }

	    region = h / 43;
	    remainder = (h - (region * 43)) * 6; 

	    p = (v * (255 - s)) >> 8;
	    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
	    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

	    switch (region) {
	        case 0:
	            r = v;
	            g = t;
	            b = p;
	            break;
	        case 1:
	            r = q;
	            g = v;
	            b = p;
	            break;
	        case 2:
	            r = p;
	            g = v;
	            b = t;
	            break;
	        case 3:
	            r = p;
	            g = q;
	            b = v;
	            break;
	        case 4:
	            r = t;
	            g = p;
	            b = v;
	            break;
	        default:
	            r = v;
	            g = p;
	            b = q;
	            break;
	    }
	    return colorFromRGB(r, g, b);
}

inline void setPixel(uint32 x, uint32 y, color_t c) {
	uint32 p;
	if (framebuffer_bpp == 1 && framebuffer_pitch == framebuffer_width) { // slight optimization for most likely configuration
		p = x + y * framebuffer_pitch;
	} else {
		p = x * framebuffer_bpp + y * framebuffer_pitch;	
	}
	ptr[p] = c;
	// ptr[p+0] = c | c >> 8 | c >> 16;  // kept in case needed later
	// ptr[p+1] = c >> 8;
	// ptr[p+2] = c >> 16;
	return;
}

void fillRect(uint32 x, uint32 y, uint32 w, uint32 h, color_t c) {
	uint32 s = 0;
	for (uint32 fy = y; fy < h+y; ++fy) {
		uint32 offset = fy * framebuffer_pitch;
		for (uint32 fx = x; fx < w+x; ++fx) {
			s = fx*framebuffer_bpp + offset;
			ptr[s] = c;
		}
		offset += framebuffer_pitch;
	}
	return;
}

void fillScreen(color_t c) {
	fillRect(0, 0, framebuffer_width, framebuffer_height, c);
}

void clearScreen() {
	cursorx = 0; 
	cursory = 0; 
	fillScreen(GColBLACK);
	cursor_pos_updated();
}

 // TODO: Better optimize this function. It works, but could be made much more
 // efficient.

void badPlaceChar(kchar ltr, uint32 x, uint32 y, color_t c) {
	for (uint8 py = 0; py < fontHeight; ++py) {
		for (uint8 px = 0; px < fontWidth+1; ++px) {
			if (font[(uint8)ltr][py]>>(fontWidth-px)&1) {
				setPixel(x+px, y+py, c);
			} else {
				setPixel(x+px, y+py, backgroundColor);
			}
		}
	}
}

void gSetCsrColor(color_t text, color_t background) {
	textColor = text;
	backgroundColor = background;
}

void gsetCsr(uint32 x, uint32 y) {
	cursorx = x;
	cursory = y;
	cursor_pos_updated();
}

void cursor_pos_updated() {
	if (FBScreen) {
		// There isn't a cursor on the framebuffer screen yet
	} else {
		uint16_t pos = cursory * terminalWidth + cursorx;
		outb(0x3D4, 0x0F);
		outb(0x3D5, (uint8) (pos & 0xFF));
		outb(0x3D4, 0x0E);
		outb(0x3D5, (uint8) ((pos >> 8) & 0xFF));
	}
}

uint32 getStrWidth(kchar *str) {
	return strlen(str) * (fontWidth+1);
}

int32 getScreenWidth() {
	return (int32)framebuffer_width;
}
int32 getScreenHeight() {
	return (int32)framebuffer_height;
}
int32 getTerminalWidth() {
	return (int32)framebuffer_width/fontWidth;
}
int32 getTerminalHeight() {
	return (int32)framebuffer_height/fontHeight;
}

void legacyScrollTerminal() {
	for (uint32 i = 0; i < terminalHeight; ++i) {
		memcpy((uint8*)&basicPtr[i * terminalWidth], (uint8*)&basicPtr[(i+1) * terminalWidth], terminalWidth*2);
	}
	cursory--;
	return;
}

void scrollTerminal() {
	for (uint32 i = 0; i < framebuffer_height/fontHeight; ++i) {
		memcpy((uint8*)(ptr+i*framebuffer_pitch*fontHeight), (uint8*)(ptr+(i+1)*framebuffer_pitch*fontHeight), framebuffer_pitch*fontHeight*framebuffer_bpp*4);
	}
	fillRect(0, framebuffer_height-fontHeight, framebuffer_width, fontHeight, backgroundColor);
	cursory--;
	return;
}

void putc(kchar chr) {
	if (chr == '\n') {
		serial_putc('\r');
		serial_putc('\n');
	} else {
		serial_putc(chr);
	}
	if (FBScreen) {
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wtype-limits"
		if (chr > 128 || chr == 0) { // kchar is a char for now, so this line causes a GCC warning
			return;
		}
		#pragma GCC diagnostic pop
		if (chr == '\n') {
			cursorx = 0;
			cursory++;
			if (cursory >= terminalHeight) {
				scrollTerminal();
			}
			return;
		}
		badPlaceChar(chr, cursorx*(fontWidth+1)+1, cursory*(fontHeight+1)+1, textColor);
		if (cursorx+1 < terminalWidth) {
			cursorx++;
		} else {
			cursorx = 0;
			cursory++;
		}
		if (cursory >= terminalHeight) {
			scrollTerminal();
		}
	} else {
		if (chr == '\n') {
			cursorx = 0;
			cursory++;
			if (cursory >= terminalHeight) {
				legacyScrollTerminal();
			}
			cursor_pos_updated();
			return;
		}
		basicPtr[cursory * terminalWidth + cursorx] = chr | (15 | 0 << 4) << 8;
		if (cursorx+1 < terminalWidth) {
			cursorx++;
		} else {
			cursorx = 0;
			cursory++;
		}
		if (cursory >= terminalHeight) {
			legacyScrollTerminal();
		}
	}
	cursor_pos_updated();
}

void termBackspace() {
	serial_putc('\b');
	serial_putc(' ');
	serial_putc('\b');
	if (FBScreen) {
		if (cursorx > 0) {
			cursorx--;
		} else if(cursory > 0) {
			cursorx = terminalWidth-1;
			cursory--;
		}
		fillRect(cursorx*(fontWidth+1)+1, cursory*(fontHeight+1)+1, fontWidth+1, fontHeight, backgroundColor); 
	} else {
		if (cursorx > 0) {
			cursorx--;
		} else if(cursory > 0) {
			cursorx = terminalWidth-1;
			cursory--;
		}
		basicPtr[cursory * terminalWidth + cursorx] = ' ' | (15 | 0 << 4) << 8;
	}
	cursor_pos_updated();
}
