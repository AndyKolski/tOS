#include <stdarg.h>
#include <system.h>
#ifndef DISPLAY_H
#define DISPLAY_H

#define GColBLACK 000|(000<<8)|(000<<16)
#define GColWHITE 255|(255<<8)|(255<<16)
#define GColRED   000|(000<<8)|(255<<16)
#define GColGREEN 000|(255<<8)|(000<<16)
#define GColBLUE  255|(000<<8)|(000<<16)


void install_display(uint64 fb_addr, uint32 fb_width, uint32 fb_height, uint8 fb_bpp, uint32 fb_pitch, bool useLegacy);

uint32 colorFromRGB(uint8 r, uint8 g, uint8 b);

void setPixel(uint32 x, uint32 y, uint32 c);

void fillRect(uint32 x, uint32 y, uint32 w, uint32 h, uint32 c);

void fillScreen(uint32 c);
void clearScreen();

// void drawDuck(uint32 dx, uint32 dy);

void badPlaceChar(kchar ltr, uint32 x, uint32 y, uint32 c);

void gSetCsrColor(uint32 text, uint32 background);

void gsetCsr(uint32 x, uint32 y);

void cursor_pos_updated();

uint32 getStrWidth(kchar *str);

int32 getScreenWidth();
int32 getScreenHeight();

void legacyScrollTerminal();

void putc(kchar chr);

void termBackspace();

#endif