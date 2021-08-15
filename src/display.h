#include <stdarg.h>
#include <system.h>

#pragma once

typedef uint32 color_t;

#define GColBLACK 000|(000<<8)|(000<<16)
#define GColWHITE 255|(255<<8)|(255<<16)
#define GColRED   000|(000<<8)|(255<<16)
#define GColGREEN 000|(255<<8)|(000<<16)
#define GColBLUE  255|(000<<8)|(000<<16)


void install_display(uint64 fb_addr, uint32 fb_width, uint32 fb_height, uint8 fb_bpp, uint32 fb_pitch, bool useLegacy);

color_t colorFromRGB(uint8 r, uint8 g, uint8 b);

#define colorFromHSV(h, s, v) _colorFromHSV(((h)*255)/360, ((s)*255)/100, ((v)*255)/100)

color_t _colorFromHSV(uint8 h, uint8 s, uint8 v);

void setPixel(uint32 x, uint32 y, color_t c);

void fillRect(uint32 x, uint32 y, uint32 w, uint32 h, color_t c);

void fillScreen(color_t c);
void clearScreen();

// void drawDuck(uint32 dx, uint32 dy);

void badPlaceChar(kchar ltr, uint32 x, uint32 y, color_t c);

void gSetCsrColor(color_t text, color_t background);

void gsetCsr(uint32 x, uint32 y);

void cursor_pos_updated();

uint32 getStrWidth(kchar *str);

uint32 getScreenWidth();
uint32 getScreenHeight();
uint32 getTerminalWidth();
uint32 getTerminalHeight();

void legacyScrollTerminal();

void putc(kchar chr);

void termBackspace();
