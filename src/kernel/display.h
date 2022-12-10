#include <system.h>

#pragma once

typedef uint32 color_t;

#define GColBLACK 000|(000<<8)|(000<<16)
#define GColWHITE 255|(255<<8)|(255<<16)
#define GColRED   000|(000<<8)|(255<<16)
#define GColGREEN 000|(255<<8)|(000<<16)
#define GColBLUE  255|(000<<8)|(000<<16)


void initDisplay();

void setPixel(uint32 xPosition, uint32 yPosition, color_t color);

void fillRect(uint32 xPosition, uint32 yPosition, uint32 width, uint32 height, color_t color);

void fillScreen(color_t color);

void clearScreen();

void placeChar(char character, uint32 xPosition, uint32 yPosition);

void setColors(color_t text, color_t background);

void setCursorPosition(uint32 xPosition, uint32 yPosition);

uint32 getScreenWidth();

uint32 getScreenHeight();

uint32 getTerminalWidth();

uint32 getTerminalHeight();

#define colorFromRGB(r, g, b) ((uint32)b|((uint32)g<<8)|((uint32)r<<16))

#define colorFromHSV(h, s, v) _colorFromHSV(((h)*255)/360, ((s)*255)/100, ((v)*255)/100)

color_t _colorFromHSV(uint8 h, uint8 s, uint8 v);

void terminalPrintChar(char character);

void terminalBackspace();
