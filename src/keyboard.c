#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "keyboard.h"
#include "stdlib.h"

struct AltKeys
{
	bool CtrlDown;
	bool AltDown;
	bool ShiftDown;
	bool CAPSDown;
	bool ESCDown;
};
struct KeyEvent
{
	bool keyDown;
	struct AltKeys Alts;
	char Key;
};

struct AltKeys CurrentKeys;

struct ASCIIKeysUpdate
{
	char ASCII;
	bool KeyDown;
	bool IsValid;
	bool IsModif;
	bool IsKeypd;
	int ModifNum;
};

struct ASCIIKeysUpdate scancodeSets[3][255];
int currentScancodeSet = 2;

#define NOMOD 0
#define CTRL 1
#define ALT 2
#define ESC 3
#define SHIFT 4
#define CAPS 5

void initKeys() {
	CurrentKeys.CtrlDown = false;
	CurrentKeys.AltDown = false;
	CurrentKeys.ShiftDown = false;
	CurrentKeys.CAPSDown = false;
	CurrentKeys.ESCDown = false;
	for (int Sc = 0; Sc < 2; ++Sc) {
		for (int code = 0; code < 255; ++code) {
			scancodeSets[Sc][code].ASCII = 0;
			scancodeSets[Sc][code].KeyDown = true;
			scancodeSets[Sc][code].IsValid = false;
			scancodeSets[Sc][code].IsModif = false;
			scancodeSets[Sc][code].ModifNum = 0;
			scancodeSets[Sc][code].IsKeypd = false;
		}
	}

	scancodeSets[2][0x01].KeyDown = true;
	scancodeSets[2][0x01].IsModif = true;
	scancodeSets[2][0x01].ModifNum = ESC;

	scancodeSets[2][0x02].KeyDown = true;
	scancodeSets[2][0x02].IsValid = true;
	scancodeSets[2][0x02].ASCII = '1';

	scancodeSets[2][0x03].KeyDown = true;
	scancodeSets[2][0x03].IsValid = true;
	scancodeSets[2][0x03].ASCII = '2';

	scancodeSets[2][0x04].KeyDown = true;
	scancodeSets[2][0x04].IsValid = true;
	scancodeSets[2][0x04].ASCII = '3';

	scancodeSets[2][0x05].KeyDown = true;
	scancodeSets[2][0x05].IsValid = true;
	scancodeSets[2][0x05].ASCII = '4';

	scancodeSets[2][0x06].KeyDown = true;
	scancodeSets[2][0x06].IsValid = true;
	scancodeSets[2][0x06].ASCII = '5';

	scancodeSets[2][0x07].KeyDown = true;
	scancodeSets[2][0x07].IsValid = true;
	scancodeSets[2][0x07].ASCII = '6';

	scancodeSets[2][0x08].KeyDown = true;
	scancodeSets[2][0x08].IsValid = true;
	scancodeSets[2][0x08].ASCII = '7';

	scancodeSets[2][0x09].KeyDown = true;
	scancodeSets[2][0x09].IsValid = true;
	scancodeSets[2][0x09].ASCII = '8';

	scancodeSets[2][0x0A].KeyDown = true;
	scancodeSets[2][0x0A].IsValid = true;
	scancodeSets[2][0x0A].ASCII = '9';

	scancodeSets[2][0x0B].KeyDown = true;
	scancodeSets[2][0x0B].IsValid = true;
	scancodeSets[2][0x0B].ASCII = '0';

	scancodeSets[2][0x0C].KeyDown = true;
	scancodeSets[2][0x0C].IsValid = true;
	scancodeSets[2][0x0C].ASCII = '-';

	scancodeSets[2][0x0D].KeyDown = true;
	scancodeSets[2][0x0D].IsValid = true;
	scancodeSets[2][0x0D].ASCII = '=';

	scancodeSets[2][0x0E].KeyDown = true;
	scancodeSets[2][0x0E].IsValid = true;
	scancodeSets[2][0x0E].ASCII = '\b';

	scancodeSets[2][0x0F].KeyDown = true;
	scancodeSets[2][0x0F].IsValid = true;
	scancodeSets[2][0x0F].ASCII = '	';

	scancodeSets[2][0x10].KeyDown = true;
	scancodeSets[2][0x10].IsValid = true;
	scancodeSets[2][0x10].ASCII = 'q';

	scancodeSets[2][0x11].KeyDown = true;
	scancodeSets[2][0x11].IsValid = true;
	scancodeSets[2][0x11].ASCII = 'w';

	scancodeSets[2][0x12].KeyDown = true;
	scancodeSets[2][0x12].IsValid = true;
	scancodeSets[2][0x12].ASCII = 'e';

	scancodeSets[2][0x13].KeyDown = true;
	scancodeSets[2][0x13].IsValid = true;
	scancodeSets[2][0x13].ASCII = 'r';

	scancodeSets[2][0x14].KeyDown = true;
	scancodeSets[2][0x14].IsValid = true;
	scancodeSets[2][0x14].ASCII = 't';

	scancodeSets[2][0x15].KeyDown = true;
	scancodeSets[2][0x15].IsValid = true;
	scancodeSets[2][0x15].ASCII = 'y';

	scancodeSets[2][0x16].KeyDown = true;
	scancodeSets[2][0x16].IsValid = true;
	scancodeSets[2][0x16].ASCII = 'u';

	scancodeSets[2][0x17].KeyDown = true;
	scancodeSets[2][0x17].IsValid = true;
	scancodeSets[2][0x17].ASCII = 'i';

	scancodeSets[2][0x18].KeyDown = true;
	scancodeSets[2][0x18].IsValid = true;
	scancodeSets[2][0x18].ASCII = 'o';

	scancodeSets[2][0x19].KeyDown = true;
	scancodeSets[2][0x19].IsValid = true;
	scancodeSets[2][0x19].ASCII = 'p';

	scancodeSets[2][0x1A].KeyDown = true;
	scancodeSets[2][0x1A].IsValid = true;
	scancodeSets[2][0x1A].ASCII = '[';

	scancodeSets[2][0x1B].KeyDown = true;
	scancodeSets[2][0x1B].IsValid = true;
	scancodeSets[2][0x1B].ASCII = ']';

	scancodeSets[2][0x1C].KeyDown = true;
	scancodeSets[2][0x1C].IsValid = true;
	scancodeSets[2][0x1C].ASCII = '\n';

	scancodeSets[2][0x1D].KeyDown = true;
	scancodeSets[2][0x1D].IsModif = true;
	scancodeSets[2][0x1D].ModifNum = CTRL;

	scancodeSets[2][0x1E].KeyDown = true;
	scancodeSets[2][0x1E].IsValid = true;
	scancodeSets[2][0x1E].ASCII = 'a';

	scancodeSets[2][0x1F].KeyDown = true;
	scancodeSets[2][0x1F].IsValid = true;
	scancodeSets[2][0x1F].ASCII = 's';

	scancodeSets[2][0x20].KeyDown = true;
	scancodeSets[2][0x20].IsValid = true;
	scancodeSets[2][0x20].ASCII = 'd';

	scancodeSets[2][0x21].KeyDown = true;
	scancodeSets[2][0x21].IsValid = true;
	scancodeSets[2][0x21].ASCII = 'f';

	scancodeSets[2][0x22].KeyDown = true;
	scancodeSets[2][0x22].IsValid = true;
	scancodeSets[2][0x22].ASCII = 'g';

	scancodeSets[2][0x23].KeyDown = true;
	scancodeSets[2][0x23].IsValid = true;
	scancodeSets[2][0x23].ASCII = 'h';

	scancodeSets[2][0x24].KeyDown = true;
	scancodeSets[2][0x24].IsValid = true;
	scancodeSets[2][0x24].ASCII = 'j';

	scancodeSets[2][0x25].KeyDown = true;
	scancodeSets[2][0x25].IsValid = true;
	scancodeSets[2][0x25].ASCII = 'k';

	scancodeSets[2][0x26].KeyDown = true;
	scancodeSets[2][0x26].IsValid = true;
	scancodeSets[2][0x26].ASCII = 'l';

	scancodeSets[2][0x27].KeyDown = true;
	scancodeSets[2][0x27].IsValid = true;
	scancodeSets[2][0x27].ASCII = ';';

	scancodeSets[2][0x28].KeyDown = true;
	scancodeSets[2][0x28].IsValid = true;
	scancodeSets[2][0x28].ASCII = '\'';

	scancodeSets[2][0x29].KeyDown = true;
	scancodeSets[2][0x29].IsValid = true;
	scancodeSets[2][0x29].ASCII = '`';

	scancodeSets[2][0x2A].KeyDown = true;
	scancodeSets[2][0x2A].IsModif = true;
	scancodeSets[2][0x2A].ModifNum = SHIFT;

	scancodeSets[2][0x2B].KeyDown = true;
	scancodeSets[2][0x2B].IsValid = true;
	scancodeSets[2][0x2B].ASCII = '\\';

	scancodeSets[2][0x2C].KeyDown = true;
	scancodeSets[2][0x2C].IsValid = true;
	scancodeSets[2][0x2C].ASCII = 'z';

	scancodeSets[2][0x2D].KeyDown = true;
	scancodeSets[2][0x2D].IsValid = true;
	scancodeSets[2][0x2D].ASCII = 'x';

	scancodeSets[2][0x2E].KeyDown = true;
	scancodeSets[2][0x2E].IsValid = true;
	scancodeSets[2][0x2E].ASCII = 'c';

	scancodeSets[2][0x2F].KeyDown = true;
	scancodeSets[2][0x2F].IsValid = true;
	scancodeSets[2][0x2F].ASCII = 'v';

	scancodeSets[2][0x30].KeyDown = true;
	scancodeSets[2][0x30].IsValid = true;
	scancodeSets[2][0x30].ASCII = 'b';

	scancodeSets[2][0x31].KeyDown = true;
	scancodeSets[2][0x31].IsValid = true;
	scancodeSets[2][0x31].ASCII = 'n';

	scancodeSets[2][0x32].KeyDown = true;
	scancodeSets[2][0x32].IsValid = true;
	scancodeSets[2][0x32].ASCII = 'm';

	scancodeSets[2][0x33].KeyDown = true;
	scancodeSets[2][0x33].IsValid = true;
	scancodeSets[2][0x33].ASCII = ',';

	scancodeSets[2][0x34].KeyDown = true;
	scancodeSets[2][0x34].IsValid = true;
	scancodeSets[2][0x34].ASCII = '.';

	scancodeSets[2][0x35].KeyDown = true;
	scancodeSets[2][0x35].IsValid = true;
	scancodeSets[2][0x35].ASCII = '/';

	scancodeSets[2][0x36].KeyDown = true;
	scancodeSets[2][0x36].IsModif = true;
	scancodeSets[2][0x36].ModifNum = SHIFT;

	scancodeSets[2][0x37].KeyDown = true;
	scancodeSets[2][0x37].IsValid = true;
	scancodeSets[2][0x37].IsKeypd = true;
	scancodeSets[2][0x37].ASCII = '*';

	scancodeSets[2][0x38].KeyDown = true;
	scancodeSets[2][0x38].IsModif = true;
	scancodeSets[2][0x38].ModifNum = ALT;

	scancodeSets[2][0x39].KeyDown = true;
	scancodeSets[2][0x39].IsValid = true;
	scancodeSets[2][0x39].ASCII = ' ';

	scancodeSets[2][0x3A].KeyDown = true;
	scancodeSets[2][0x3A].IsModif = true;
	scancodeSets[2][0x3A].ModifNum = CAPS;

	scancodeSets[2][0xAA].KeyDown = false;
	scancodeSets[2][0xAA].IsModif = true;
	scancodeSets[2][0xAA].ModifNum = SHIFT;

	scancodeSets[2][0xB6].KeyDown = false;
	scancodeSets[2][0xB6].IsModif = true;
	scancodeSets[2][0xB6].ModifNum = SHIFT;
}

int getKey() {
	int c;

	c = inb(0x64);
	if ((c & 1) == 0) {
		return(0);
	}
	c = inb(0x60);
	if (scancodeSets[currentScancodeSet][c].IsModif) {
		if (scancodeSets[currentScancodeSet][c].ModifNum == CTRL) {
			CurrentKeys.CtrlDown = scancodeSets[currentScancodeSet][c].KeyDown;
		} else if (scancodeSets[currentScancodeSet][c].ModifNum == ALT) {
			CurrentKeys.AltDown = scancodeSets[currentScancodeSet][c].KeyDown;
		} else if (scancodeSets[currentScancodeSet][c].ModifNum == ESC) {
			CurrentKeys.ESCDown = scancodeSets[currentScancodeSet][c].KeyDown;
		} else if (scancodeSets[currentScancodeSet][c].ModifNum == SHIFT) {
			CurrentKeys.ShiftDown = scancodeSets[currentScancodeSet][c].KeyDown;
		}
	}
	return((c));
}

char getASCIICode() {
	int k = getKey();
	if (scancodeSets[currentScancodeSet][k].IsValid && scancodeSets[currentScancodeSet][k].KeyDown && scancodeSets[currentScancodeSet][k].IsModif == false) {
		if (CurrentKeys.ShiftDown) {
			return scancodeSets[currentScancodeSet][k].ASCII + ('A' - 'a');
		}
		return scancodeSets[currentScancodeSet][k].ASCII;
	} else {
		return 0;
	}
}
char sendKeyboardCommand(char command, char data, bool allowACKResp) {
	int OK = false;
	int count = 0;
	while (OK == false && count < 5) {
		count++;
		outb(0x60, command);
		unsigned char response = inb(0x60);
		if (response == 0xFA) {
			outb(0x60, data);
			while (true) {
				unsigned char in = inb(0x60);
				if ((in != 0xFA && in != 0x00) || allowACKResp) {
					return in;
				}
			}
		}
	}
	return 0;
}