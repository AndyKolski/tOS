#include <system.h>

#pragma once

typedef uint32 color_t;

void initDisplay();

void setPixel(uint32 xPosition, uint32 yPosition, color_t color);

void fillRect(uint32 xPosition, uint32 yPosition, uint32 width, uint32 height, color_t color);

void fillScreen(color_t color);

void clearScreen();

void placeChar(char character, uint32 xPosition, uint32 yPosition);

void setColors(color_t text, color_t background);

void setCursorPosition(uint32 xPosition, uint32 yPosition);

uint32 getCursorXPosition();

uint32 getCursorYPosition();

uint32 getScreenWidth();

uint32 getScreenHeight();

uint32 getTerminalWidth();

uint32 getTerminalHeight();

#define colorFromRGB(r, g, b) ((uint32)b | ((uint32)g << 8) | ((uint32)r << 16))

#define colorFromHSV(h, s, v) _colorFromHSV(((h) * 255) / 360, ((s) * 255) / 100, ((v) * 255) / 100)

color_t _colorFromHSV(uint8 h, uint8 s, uint8 v);

void terminalPrintChar(char character);

void terminalBackspace();

#define G_COLOR_BLACK colorFromRGB(0, 0, 0)
#define G_COLOR_WHITE colorFromRGB(255, 255, 255)
#define G_COLOR_RED   colorFromRGB(255, 0, 0)
#define G_COLOR_GREEN colorFromRGB(0, 255, 0)
#define G_COLOR_BLUE  colorFromRGB(0, 0, 255)

#define BIOS_COLOR_BLACK        0x00
#define BIOS_COLOR_BLUE         0x01
#define BIOS_COLOR_GREEN        0x02
#define BIOS_COLOR_CYAN         0x03
#define BIOS_COLOR_RED          0x04
#define BIOS_COLOR_MAGENTA      0x05
#define BIOS_COLOR_BROWN        0x06
#define BIOS_COLOR_LIGHTGRAY    0x07
#define BIOS_COLOR_DARKGRAY     0x08
#define BIOS_COLOR_LIGHTBLUE    0x09
#define BIOS_COLOR_LIGHTGREEN   0x0A
#define BIOS_COLOR_LIGHTCYAN    0x0B
#define BIOS_COLOR_LIGHTRED     0x0C
#define BIOS_COLOR_LIGHTMAGENTA 0x0D
#define BIOS_COLOR_YELLOW       0x0E
#define BIOS_COLOR_WHITE        0x0F
