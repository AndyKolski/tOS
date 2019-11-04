#include <header.h>
//#include <Duck.h>
#include <font.h>
#include <stdarg.h>

#define GColBLACK 0|(0<<8)|(0<<16)
#define GColRED 0|(0<<8)|(255<<16)
#define GColGREEN 0|(255<<8)|(0<<16)
#define GColBLUE 255|(0<<8)|(0<<16)
#define GColWHITE 255|(255<<8)|(255<<16)

uint8 *ptr;
uint32 framebuffer_width;
uint32 framebuffer_height;
uint8 framebuffer_bpp;
uint32 framebuffer_pitch;

uint32 cursorx = 0;
uint32 cursory = 0;

uint32 textColor = GColWHITE;
uint32 backgroundColor = GColBLACK;

uint8 fontWidth = 5;
uint8 fontHeight = 11;


void install_display(uint64 fb_addr, uint32 fb_width, uint32 fb_height, uint8 fb_bpp, uint32 fb_pitch) {
	ptr = (uint8 *) fb_addr; 
	framebuffer_width = fb_width;
	framebuffer_height = fb_height;
	framebuffer_bpp = fb_bpp/8;
	framebuffer_pitch = fb_pitch;
	fillRect(0,0,fb_width,fb_height,GColBLACK);
	printf("Created console with size %ix%i (%ipx x %ipx @ %ibpp)\n", framebuffer_width/(fontWidth+1), framebuffer_height/(fontHeight+1), framebuffer_width, framebuffer_height, fb_bpp);
}

uint32 colorFromRGB(uint8 r, uint8 g, uint8 b) {
	return b|(g<<8)|(r<<16);
}

void setPixel(uint32 x, uint32 y, uint32 c) {
	uint32 p = x * framebuffer_bpp + y * framebuffer_pitch;
	ptr[p] = c;
	ptr[p+1] = c >> 8;
	ptr[p+2] = c >> 16;
	return;
}
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
void putc(kchar chr) {
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

			kchar buf[32] = {0};
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
					itoa(va_arg(lList,uint64), buf, 16);
					puts(buf);
					break; 
							
				case 'b': //Binary representation
					itoa(va_arg(lList,uint64), buf, 2);
					puts(buf);
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
	if (cursorx > 0) {
		cursorx--;
	} else if(cursory > 0) {
		cursorx = (framebuffer_width/(fontWidth+1))-1;
		cursory--;
	}
	fillRect(cursorx*(fontWidth+1)+1, cursory*(fontHeight+1)+1, fontWidth+1, fontHeight, backgroundColor);
}

/*void drawDuck() {
	//640x426x32
	for (int x = 1; x < 640; ++x)
	{
		for (int y = 1; y < 426; ++y)
		{
			uint32 dp = x * 4 + y * (4*640);
			uint32 p = x * framebuffer_bpp + y * framebuffer_pitch;
			ptr[p] = duck_map[dp];
			ptr[p+1] = duck_map[dp+1];
			ptr[p+2] = duck_map[dp+2];
		}
	}
}*/
void fillRect(uint32 x, uint32 y, uint32 w, uint32 h, uint32 c) {
	int s = 0;
	for (int fy = y; fy < h+y; ++fy) {
		uint32 offset = fy * framebuffer_pitch;
		for (int fx = x; fx < w+x; ++fx) {
			s = fx*framebuffer_bpp + offset;
			ptr[s] = c;
			ptr[s + 1] = c >> 8;
			ptr[s + 2] = c >> 16;
		}
		offset += framebuffer_pitch;
	}
	return;
}