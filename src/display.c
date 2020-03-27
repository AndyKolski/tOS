#include <display.h>
#include <font.h>
#include <libs.h>
#include <serial.h>
#include <stdarg.h>
#include <system.h>
//#include <Duck.h>

uint32 *ptr;
uint32 framebuffer_width = 0;
uint32 framebuffer_height = 0;
uint8 framebuffer_bpp = 0;
uint32 framebuffer_pitch = 0;

uint16 *basicPtr;

uint32 cursorx = 0;
uint32 cursory = 0;

uint32 textColor = GColWHITE;
uint32 backgroundColor = GColBLACK;

uint8 fontWidth = 5;
uint8 fontHeight = 11;

bool FBScreen = false;


void install_display(uint64 fb_addr, uint32 fb_width, uint32 fb_height, uint8 fb_bpp, uint32 fb_pitch, bool useLegacy) {
	FBScreen = !useLegacy;
	if (FBScreen) {
		ptr = (uint32 *)(uint64*)fb_addr; 
		framebuffer_width = fb_width;
		framebuffer_height = fb_height;
		framebuffer_bpp = fb_bpp/8/4;
		framebuffer_pitch = fb_pitch/4;
		fillRect(0,0,fb_width,fb_height,GColBLACK);
		printf("Created console with size %ix%i (%ipx x %ipx @ %ibpp)\n", framebuffer_width/(fontWidth+1), framebuffer_height/(fontHeight+1), framebuffer_width, framebuffer_height, fb_bpp);
	} else {
		basicPtr = (uint16*) 0x000b8000;
		framebuffer_width = 80;
		framebuffer_height = 25;
		for (uint8 y = 0; y < 25; y++) {
			for (uint8 x = 0; x < 80; x++) {
				basicPtr[y * 80 + x] = ' ' | (15 | 0 << 4) << 8;
			}
		}
		printf("Created console with size %ix%i (--px x --px @ --bpp)\n", framebuffer_width, framebuffer_height);
	}}

uint32 colorFromRGB(uint8 r, uint8 g, uint8 b) {
	return b|(g<<8)|(r<<16);
}

void setPixel(uint32 x, uint32 y, uint32 c) {
	uint32 p;
	if (framebuffer_bpp == 1 && framebuffer_pitch == framebuffer_width) { //slight optimization for most likely configuration
		p = x + y * framebuffer_pitch;
	} else {
		p = x * framebuffer_bpp + y * framebuffer_pitch;	
	}
	ptr[p] = c;
	// ptr[p+0] = c | c >> 8 | c >> 16;  //kept in case needed later
	// ptr[p+1] = c >> 8;
	// ptr[p+2] = c >> 16;
	return;
}
void fillRect(uint32 x, uint32 y, uint32 w, uint32 h, uint32 c) {
	int s = 0;
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
// void drawDuck(uint32 dx, uint32 dy) {
// 	//640x426x32
// 	uint32 *duck_map_32 = &duck_map;
// 	for (int y = 0; y < 426; y++) {
// 		for (int x = 0; x < 640; x++) {
// 			ptr[(x+dx) + (y+dy) * framebuffer_pitch] = duck_map_32[x + y * 640];
// 		}
// 	}
// }

void badPlaceChar(kchar ltr, uint32 x, uint32 y, uint32 c) {
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
void badPlaceStr(kchar *str, uint32 x, uint32 y, uint32 c) {
	for (int i = 0; i < strlen(str); ++i) {
		badPlaceChar(str[i], x + (i*(fontWidth+1)), y, c);
	}
}
void gSetCsrColor(uint32 text, uint32 background) {
	textColor = text;
	backgroundColor = background;
}

void gsetCsr(uint32 x, uint32 y) {
	cursorx = x;
	cursory = y;
}
uint32 getStrWidth(kchar *str) {
	return strlen(str) * (fontWidth+1);
}
void legacyScroll() {
	for (uint32 i = 0; i < framebuffer_height; ++i) {
		memcpy((uint8*)&basicPtr[i * framebuffer_width], (uint8*)&basicPtr[(i+1) * framebuffer_width], framebuffer_width*2);
	}
	cursory--;
}

void putc(kchar chr) {
	if (chr == '\n') {
		serial_putc('\r');
		serial_putc('\n');
	} else {
		serial_putc(chr);
	}
	if (FBScreen) {
		if (chr > 128 || chr == 0) {
			return;
		}
		if (chr == '\n') {
			cursorx = 0;
			cursory++;
			return;
		}
		badPlaceChar(chr, cursorx*(fontWidth+1)+1, cursory*(fontHeight+1)+1, textColor);
		if (cursorx+1 < framebuffer_width/(fontWidth+1)) {
			cursorx++;
		} else {
			cursorx = 0;
			cursory++;
		}
	} else {
		if (chr == '\n') {
			cursorx = 0;
			cursory++;
			if (cursory >= framebuffer_height) {
				legacyScroll();
			}
			return;
		}
		basicPtr[cursory * framebuffer_width + cursorx] = chr | (15 | 0 << 4) << 8;
		if (cursorx+1 < framebuffer_width) {
			cursorx++;
		} else {
			cursorx = 0;
			cursory++;
		}
		if (cursory >= framebuffer_height) {
			legacyScroll();
		}
	}
}
void puts(kchar *text) {
	for (int i = 0; i < strlen(text); ++i) {
		putc(text[i]);
	}
}

void formatAndPrintString(kchar *pcFmt, va_list lList) { 
	while(*pcFmt != '\0') {
		if(*pcFmt == '%') {
			pcFmt++; //lList

			kchar buf[66] = {0};
			switch (*pcFmt) {
				case 'c' : //char
					putc((kchar)va_arg(lList,int));
					break; 
							
				case 'i' : //int
					itoa(va_arg(lList,int32), buf, 10);
					puts(buf);
					break;
							
				case 'u' : //unsigned int
					itoa(va_arg(lList,uint32), buf, 10);
					puts(buf);
					break;
							
				case 'l' : //long int
					itoa(va_arg(lList,uint64), buf, 10);
					puts(buf);
					break;
							
				case 's': //String
					puts(va_arg(lList,kchar *));
					break; 
							
				case 'x': //Hexadecimal representation
					itoa(va_arg(lList,uint32), buf, 16);
					puts(buf);
					break; 
							
				case 'b': //Binary representation
					itoa(va_arg(lList,uint32), buf, 2);
					puts(buf);
					break; 
							
				case 'B': //Boolean
					puts(va_arg(lList,bool) ? "True" : "False");
					break; 

				case '%':
					putc('%');
					break; 

				default:
					break;
			}
			pcFmt++;
		} else {
		   putc(*pcFmt);
		   pcFmt++;
		}
	}
	return;
}

void printf(kchar *pcFormat, ...) {
	if(!pcFormat) {
		return;
	}
	va_list lList;
	va_start(lList, pcFormat);
	formatAndPrintString(pcFormat, lList);
	return;
}

void termBackspace() {
	if (FBScreen) {
		if (cursorx > 0) {
			cursorx--;
		} else if(cursory > 0) {
			cursorx = (framebuffer_width/(fontWidth+1))-1;
			cursory--;
		}
		fillRect(cursorx*(fontWidth+1)+1, cursory*(fontHeight+1)+1, fontWidth+1, fontHeight, backgroundColor); 
	} else {
		if (cursorx > 0) {
			cursorx--;
		} else if(cursory > 0) {
			cursorx = (framebuffer_width/(fontWidth+1))-1;
			cursory--;
		}
		basicPtr[cursory * framebuffer_width + cursorx] = ' ' | (15 | 0 << 4) << 8;
	}
}
