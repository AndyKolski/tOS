#include <header.h>
//#include <Duck.h>
#include <font.h>

char *ptr;
uint32 framebuffer_width;
uint32 framebuffer_height;
uint8 framebuffer_bpp;
uint32 framebuffer_pitch;

void install_display(uint64 fb_addr, uint32 fb_width, uint32 fb_height, uint8 fb_bpp, uint32 fb_pitch) {
	ptr = (char *) fb_addr; 
	framebuffer_width = fb_width;
	framebuffer_height = fb_height;
	framebuffer_bpp = fb_bpp/8;
	framebuffer_pitch = fb_pitch;
	fillRect(0,0,fb_width,fb_height,0);
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
void badPlaceChar(uint8 ltr, uint32 x, uint32 y, uint32 c) {
	for (int i = 0; i < 11; ++i) {
		for (int p = 0; p < 8; ++p) {
			if (font[ltr][i]>>(5-p)&1) {
				setPixel(x+p, y+i, c);
			}
		}
	}
}
void badPlaceCharBkg(uint8 ltr, uint32 x, uint32 y, uint32 c, uint32 bkc) {
	for (int i = 0; i < 11; ++i) {
		for (int p = 0; p < 8; ++p) {
			if (font[ltr][i]>>(5-p)&1) {
				setPixel(x+p, y+i, c);
			} else {
				setPixel(x+p, y+i, bkc);
			}
		}
	}
}
void badPlaceStr(uint8 *str, uint32 x, uint32 y, uint32 c) {
	for (int i = 0; i < strlen(str); ++i) {
		badPlaceChar(str[i], x + (i*6), y, c);
	}
}
void badPlaceStrBkg(uint8 *str, uint32 x, uint32 y, uint32 c, uint32 bkc) {
	for (int i = 0; i < strlen(str); ++i) {
		badPlaceCharBkg(str[i], x + (i*6), y, c, bkc);
	}
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