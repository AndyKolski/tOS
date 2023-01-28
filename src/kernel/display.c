#include <display.h>
#include <font.h>
#include <io.h>
#include <serial.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <system.h>
#include <intmath.h>
#include <multibootdata.h>

 // TODO: The VESA framebuffer needs to be double-buffered, and it needs a 
 // cursor, among other fixes.

 // TODO: Lots of this code is old, badly written, and generally not great. I
 // should rewrite a good portion of it at some point.

uint32 *graphicFB; // pointer to the graphical framebuffer
uint16 *BIOSFB; // pointer to the BIOS text console framebuffer

uint32 framebuffer_width = 0; // framebuffer width, in pixels
uint32 framebuffer_height = 0; // framebuffer height, in pixels
uint8 framebuffer_depth = 0; // the number of bytes per pixel
uint32 framebuffer_pitch = 0; // the number of bytes between one pixel and the one directly below it

uint8 framebuffer_depth_doublewords = 0; // the number of doublewords per pixel
uint32 framebuffer_pitch_doublewords = 0; // the number of doublewords between one pixel and the one directly below it

uint32 terminalWidth = 0; // the width of the terminal, in characters
uint32 terminalHeight = 0; // the height of the terminal, in characters

uint32 lineHeight = 0; // the height of a line of text, in pixels (fontHeight + 1)

uint32 cursorX = 0; // the horizontal position of the cursor on the terminal, in characters
uint32 cursorY = 0; // the vertical position of the cursor on the terminal, in characters

bool FBScreen = false; // is the display graphical or a text console

bool isDisplayInitialized = false;

color_t textColor = GColWHITE;
color_t backgroundColor = GColBLACK;

void initDisplay() {

	displayData_t displayData = *getDisplayData();

	FBScreen = displayData.isGraphicalFramebuffer;

	if (FBScreen) { // We have a graphical framebuffer
		graphicFB = displayData.framebufferVirtAddress;
		
		framebuffer_width = displayData.width;
		framebuffer_height = displayData.height;
		framebuffer_depth = displayData.depth;
		framebuffer_pitch = displayData.pitch;

		framebuffer_depth_doublewords = framebuffer_depth / 4;
		framebuffer_pitch_doublewords = framebuffer_pitch / 4;

		lineHeight = fontHeight + 1;
		terminalWidth = framebuffer_width/(fontWidth+1);
		terminalHeight = framebuffer_height/(lineHeight);

		clearScreen();
		isDisplayInitialized = true;
		printf("Created console with size %u x %u (%u px x %u px @ %i bpp) at 0x%x (virt 0x%p) (%lu %s)\n", terminalWidth, terminalHeight, framebuffer_width, framebuffer_height, displayData.depth * 8, displayData.framebufferPhysAddress, graphicFB, numBytesToHuman(displayData.framebufferSize), numBytesToUnit(displayData.framebufferSize));
	} else { // We have a BIOS text console
		BIOSFB = displayData.framebufferVirtAddress;
		terminalWidth = displayData.width;
		terminalHeight = displayData.height;
		for (uint8 y = 0; y < 25; y++) {
			for (uint8 x = 0; x < 80; x++) {
				BIOSFB[y * 80 + x] = ' ' | (15 | 0 << 4) << 8;
			}
		}
		isDisplayInitialized = true;
		printf("Created console with size %ux%u\n", terminalWidth, terminalHeight);
	}
}

/// @brief Sets the pixel at the specified position to the specified color. This function does not check if the pixel is within the bounds of the framebuffer.
/// @param xPosition The horizontal position of the pixel to set, in pixels.
/// @param yPosition The vertical position of the pixel to set, in pixels.
/// @param color The color to set the pixel to.
inline void setPixel_UNSAFE(uint32 xPosition, uint32 yPosition, color_t color) {
	graphicFB[xPosition + yPosition * framebuffer_pitch_doublewords] = color;
}

/// @brief Sets the pixel at the specified position to the specified color. This function checks if the pixel is within the bounds of the framebuffer.
/// @param xPosition The horizontal position of the pixel to set, in pixels.
/// @param yPosition The vertical position of the pixel to set, in pixels.
/// @param color The color to set the pixel to.
inline void setPixel(uint32 xPosition, uint32 yPosition, color_t color) {
	if (!isDisplayInitialized) {
		return;
	}

	if (xPosition >= framebuffer_width || yPosition >= framebuffer_height) {
		return;
	}

	setPixel_UNSAFE(xPosition, yPosition, color);
}

/// @brief Fills in a rectangle of the specified size at the specified position with the specified color. This function does not check if the rectangle is within the bounds of the framebuffer.
/// @param xPosition The horizontal position of the top-left corner of the rectangle to fill, in pixels.
/// @param yPosition The vertical position of the top-left corner of the rectangle to fill, in pixels.
/// @param width The width of the rectangle to fill, in pixels.
/// @param height The height of the rectangle to fill, in pixels.
/// @param color The color to fill the rectangle with.
void fillRect_UNSAFE(uint32 xPosition, uint32 yPosition, uint32 width, uint32 height, color_t color) {
	uint32 verticalEnd = (yPosition + height) * framebuffer_pitch_doublewords;
	for (uint32 verticalOffset = yPosition * framebuffer_pitch_doublewords; verticalOffset < verticalEnd; verticalOffset += framebuffer_pitch_doublewords) {
		for (uint32 horizontalOffset = xPosition; horizontalOffset < xPosition + width; horizontalOffset++) {
			graphicFB[horizontalOffset + verticalOffset] = color;
		}
	}
}

/// @brief Fills in a rectangle of the specified size at the specified position with the specified color. This function checks if the rectangle is within the bounds of the framebuffer.
/// @param xPosition The horizontal position of the top-left corner of the rectangle to fill, in pixels.
/// @param yPosition The vertical position of the top-left corner of the rectangle to fill, in pixels.
/// @param width The width of the rectangle to fill, in pixels.
/// @param height The height of the rectangle to fill, in pixels.
/// @param color The color to fill the rectangle with.
void fillRect(uint32 xPosition, uint32 yPosition, uint32 width, uint32 height, color_t color) {
	if (!isDisplayInitialized) {
		return;
	}

	if (xPosition >= framebuffer_width || yPosition >= framebuffer_height) {
		return;
	}

	if (xPosition + width > framebuffer_width) {
		return;
	}

	if (yPosition + height > framebuffer_height) {
		return;
	}
	
	fillRect_UNSAFE(xPosition, yPosition, width, height, color);
}

/// @brief Fills the entire screen with the specified color.
/// @param color The color to fill the screen with.
void fillScreen(color_t color) {
	fillRect_UNSAFE(0, 0, framebuffer_width, framebuffer_height, color);
}

void clearScreen() {
	fillScreen(backgroundColor);
	setCursorPosition(0, 0);
}

/// @brief Place a character on the screen at the specified position.
/// @param character The character to place on the screen.
/// @param xPosition The horizontal position of the character to place, in pixels.
/// @param yPosition The vertical position of the character to place, in pixels.
void placeChar(char character, uint32 xPosition, uint32 yPosition) {
	if (!isDisplayInitialized) {
		return;
	}
	if (xPosition >= framebuffer_width || yPosition >= framebuffer_height) {
		return;
	}
	if (xPosition + fontWidth > framebuffer_width) {
		return;
	}
	if (yPosition + fontHeight > framebuffer_height) {
		return;
	}
	color_t setColor = (color_t) character;
	uint32 verticalOffset = yPosition * framebuffer_pitch_doublewords;
	for (uint32 loopYPosition = yPosition; loopYPosition < fontHeight + yPosition; loopYPosition++) {
		for (uint32 loopXPosition = xPosition; loopXPosition < fontWidth + 1u + xPosition; loopXPosition++) {
			if (font[(uint8)character][loopYPosition-yPosition] >> (fontWidth - (loopXPosition - xPosition)) & 1) {
				setColor = textColor;
			} else {
				setColor = backgroundColor;
			}
			graphicFB[verticalOffset + loopXPosition] = setColor;
		}
		verticalOffset += framebuffer_pitch_doublewords;
	}
}

			


void cursorMoved() {
	if (!isDisplayInitialized) {
		return;
	}
	if (FBScreen) {
		// There isn't a cursor on the framebuffer screen yet
	} else {
		uint16 pos = cursorY * terminalWidth + cursorX;
		outb(0x3D4, 0x0F);
		outb(0x3D5, (uint8) (pos & 0xFF));
		outb(0x3D4, 0x0E);
		outb(0x3D5, (uint8) ((pos >> 8) & 0xFF));
	}
}

void setColors(color_t text, color_t background) {
	textColor = text;
	backgroundColor = background;
}
void setCursorPosition(uint32 xPosition, uint32 yPosition) {
	cursorX = xPosition;
	cursorY = yPosition;
	cursorMoved();
}

uint32 getScreenWidth() {
	return framebuffer_width;
}
uint32 getScreenHeight() {
	return framebuffer_height;
}
uint32 getTerminalWidth() {
	return framebuffer_width/fontWidth;
}
uint32 getTerminalHeight() {
	return framebuffer_height/fontHeight;
}




void scrollBIOS() {
	if (!isDisplayInitialized) {
		return;
	}
	for (uint32 i = 0; i < terminalHeight; ++i) {
		memcpy((uint8*)&BIOSFB[i * terminalWidth], (uint8*)&BIOSFB[(i+1) * terminalWidth], terminalWidth*2);
	}
	cursorY--;
	return;
}

void scrollTerminal() {
	if (!isDisplayInitialized) {
		return;
	}

	// For each line of text in the framebuffer, we copy it to the line above it
	for (uint32 i = 0; i < terminalHeight-1; i++) {
		memcpy(
			(uint8*)(graphicFB + (i * (lineHeight) * framebuffer_pitch_doublewords)),
			(uint8*)(graphicFB + ((i+1) * (lineHeight) * framebuffer_pitch_doublewords)),
			framebuffer_pitch * lineHeight
		);
	}

	// We fill the bottom line with blank space
	fillRect(0, framebuffer_height - lineHeight, framebuffer_width, lineHeight, backgroundColor);

	cursorY--;
	return;
}

void terminalPrintChar(char character) {
	if (!isDisplayInitialized) {
		return;
	}
	if (FBScreen) {
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wtype-limits"
		if (character > 128 || character == 0) { // char is a char for now, so this line causes a GCC warning
			return;
		}
		#pragma GCC diagnostic pop
		if (character == '\n') {
			cursorX = 0;
			cursorY++;
			if (cursorY >= terminalHeight) {
				scrollTerminal();
			}
			return;
		}
		placeChar(character, cursorX*(fontWidth+1)+1, cursorY*(fontHeight+1)+1);
		if (cursorX+1 < terminalWidth) {
			cursorX++;
		} else {
			cursorX = 0;
			cursorY++;
		}
		if (cursorY >= terminalHeight) {
			scrollTerminal();
		}
	} else {
		if (character == '\n') {
			cursorX = 0;
			cursorY++;
			if (cursorY >= terminalHeight) {
				scrollBIOS();
			}
			cursorMoved();
			return;
		}
		BIOSFB[cursorY * terminalWidth + cursorX] = character | (15 | 0 << 4) << 8;
		if (cursorX+1 < terminalWidth) {
			cursorX++;
		} else {
			cursorX = 0;
			cursorY++;
		}
		if (cursorY >= terminalHeight) {
			scrollBIOS();
		}
	}
	cursorMoved();
}

void terminalBackspace() {
	if (!isDisplayInitialized) {
		return;
	}
	if (FBScreen) {
		if (cursorX > 0) {
			cursorX--;
		} else if(cursorY > 0) {
			cursorX = terminalWidth-1;
			cursorY--;
		}
		fillRect(cursorX*(fontWidth+1)+1, cursorY*(fontHeight+1)+1, fontWidth+1, fontHeight, backgroundColor); 
	} else {
		if (cursorX > 0) {
			cursorX--;
		} else if(cursorY > 0) {
			cursorX = terminalWidth-1;
			cursorY--;
		}
		BIOSFB[cursorY * terminalWidth + cursorX] = ' ' | (15 | 0 << 4) << 8;
	}
	cursorMoved();
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