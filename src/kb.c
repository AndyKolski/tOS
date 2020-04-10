#include <display.h>
#include <io.h>
#include <irq.h>
#include <libs.h>
#include <system.h>

#define NOMOD 0
#define CTRL 1
#define ALT 2
#define ESC 3
#define SHIFT 4
#define CAPS 5

#define NOA 0 //No ASCII representation

typedef struct scancodeEvent {
	uint8 ASCII;
	uint8 UpperASCII;
	bool CanUpper;
	bool IsKeyDown;
	bool CanPrint;
	bool IsModif;
	bool IsKeypd;
	int ModifNum;
} scancodeEvent;

typedef struct modKeyState {
	bool CtrlDown;
	bool AltDown;
	bool ShiftDown;
	bool CAPSDown;
	bool ESCDown;
} modKeyState;

modKeyState modState;

volatile bool isPendingKeyEvent = false;
volatile kchar pendingKeyEvent = 0;

scancodeEvent codeSets[4][255] = { 
	{
		//0th, leave blank
	},
	{
		//1st codeset

		//ASC,UPA, CU,KD,CP,IM,IK,MN
		// L ASCII
		//     L Uppercase ASCII
		//          L Can this key be uppercase?
		//             L Is this a key-down event?
		//                L Is this key printable?
		//                   L Is this a modifier key?
		//                      L Is this key on the number pad
		//                        L Key modifier number
		{0},
		{NOA, NOA, 0, 1, 0, 1, 0, ESC}, //esc down 0x01
		{'1', '!', 1, 1, 1, 0, 0, 0}, //[1 !] down 0x02
		{'2', '@', 1, 1, 1, 0, 0, 0}, //[2 @] down 0x03
		{'3', '#', 1, 1, 1, 0, 0, 0}, //[3 #] down 0x04
		{'4', '$', 1, 1, 1, 0, 0, 0}, //[4 $] down 0x05
		{'5', '%', 1, 1, 1, 0, 0, 0}, //[5 %] down 0x06
		{'6', '^', 1, 1, 1, 0, 0, 0}, //[6 ^] down 0x07
		{'7', '&', 1, 1, 1, 0, 0, 0}, //[7 &] down 0x08
		{'8', '*', 1, 1, 1, 0, 0, 0}, //[8 *] down 0x09
		{'9', '(', 1, 1, 1, 0, 0, 0}, //[9 (] down 0x0A
		{'0', ')', 1, 1, 1, 0, 0, 0}, //[0 )] down 0x0B
		{'-', '_', 1, 1, 1, 0, 0, 0}, //[- _] down 0x0C
		{'=', '+', 1, 1, 1, 0, 0, 0}, //[= +] down 0x0D
		{'\b',NOA, 0, 1, 1, 0, 0, 0}, //backs down 0x0E
		{' ', NOA, 0, 1, 1, 0, 0, 0}, //space down 0x0F
		{'q', 'Q', 1, 1, 1, 0, 0, 0}, //[q Q] down 0x10
		{'w', 'W', 1, 1, 1, 0, 0, 0}, //[w W] down 0x11
		{'e', 'E', 1, 1, 1, 0, 0, 0}, //[e E] down 0x12
		{'r', 'R', 1, 1, 1, 0, 0, 0}, //[r R] down 0x13
		{'t', 'T', 1, 1, 1, 0, 0, 0}, //[t T] down 0x14
		{'y', 'Y', 1, 1, 1, 0, 0, 0}, //[y Y] down 0x15
		{'u', 'U', 1, 1, 1, 0, 0, 0}, //[u U] down 0x16
		{'i', 'I', 1, 1, 1, 0, 0, 0}, //[i I] down 0x17
		{'o', 'O', 1, 1, 1, 0, 0, 0}, //[o O] down 0x18
		{'p', 'P', 1, 1, 1, 0, 0, 0}, //[p P] down 0x19
		{'[', '{', 1, 1, 1, 0, 0, 0}, //[[ {] down 0x1A
		{']', '}', 1, 1, 1, 0, 0, 0}, //[] }] down 0x1B
		{'\n',NOA, 0, 1, 1, 0, 0, 0}, //enter down 0x0C
		{NOA, NOA, 0, 1, 0, 1, 0, CTRL},//ctl down 0x1D
		{'a', 'A', 1, 1, 1, 0, 0, 0}, //[a A] down 0x1E
		{'s', 'S', 1, 1, 1, 0, 0, 0}, //[s S] down 0x1F
		{'d', 'D', 1, 1, 1, 0, 0, 0}, //[d D] down 0x20
		{'f', 'F', 1, 1, 1, 0, 0, 0}, //[f F] down 0x21
		{'g', 'G', 1, 1, 1, 0, 0, 0}, //[g G] down 0x22
		{'h', 'H', 1, 1, 1, 0, 0, 0}, //[h H] down 0x23
		{'j', 'J', 1, 1, 1, 0, 0, 0}, //[j J] down 0x24
		{'k', 'K', 1, 1, 1, 0, 0, 0}, //[k K] down 0x25
		{'l', 'L', 1, 1, 1, 0, 0, 0}, //[l L] down 0x26
		{';', ':', 1, 1, 1, 0, 0, 0}, //[; :] down 0x27
		{'\'','"', 1, 1, 1, 0, 0, 0}, //[' "] down 0x28
		{'`', '~', 1, 1, 1, 0, 0, 0}, //[` ~] down 0x29
		{NOA, NOA, 0, 1, 0,1,0,SHIFT},//shift down 0x2A
		{'\\','|', 1, 1, 1, 0, 0, 0}, //[\ |] down 0x2B
		{'z', 'Z', 1, 1, 1, 0, 0, 0}, //[z Z] down 0x2C
		{'x', 'X', 1, 1, 1, 0, 0, 0}, //[x X] down 0x2D
		{'c', 'C', 1, 1, 1, 0, 0, 0}, //[c C] down 0x2E
		{'v', 'V', 1, 1, 1, 0, 0, 0}, //[v V] down 0x2F
		{'b', 'B', 1, 1, 1, 0, 0, 0}, //[b B] down 0x30
		{'n', 'N', 1, 1, 1, 0, 0, 0}, //[n N] down 0x31
		{'m', 'M', 1, 1, 1, 0, 0, 0}, //[m M] down 0x32
		{',', '<', 1, 1, 1, 0, 0, 0}, //[, <] down 0x33
		{'.', '>', 1, 1, 1, 0, 0, 0}, //[. >] down 0x34
		{'/', '?', 1, 1, 1, 0, 0, 0}, //[/ ?] down 0x35
		{NOA, NOA, 0, 1, 0,1,0,SHIFT},//shift down 0x36
		{'*', NOA, 0, 1, 1, 0, 1, 0}, // [*]  down 0x37
		{NOA, NOA, 0, 1, 0, 1, 0, ALT}, //alt down 0x38
		{' ', NOA, 0, 1, 1, 0, 0, 0}, //space down 0x39
		{NOA, NOA, 0, 1, 0, 1, 0,CAPS},//caps down 0x2A
	}, 
	{
		//2nd
	},
	{
		//3rd
	}
};

uint8 currentScancode = 1;

/* Handles the keyboard interrupt */
void keyboard_handler(struct regs *r __attribute__((__unused__))) {
	uint8 scancode = inportb(0x60);
	scancodeEvent event = codeSets[currentScancode][scancode];

	if(event.IsModif) {
		if (event.ModifNum == SHIFT) {
			modState.ShiftDown = event.IsKeyDown;
		}
		if (event.ModifNum == ESC && modState.ShiftDown) {
			reboot();
		}
	}
	if (event.ASCII == '\b') {
		termBackspace();
		return;
	}
	if (event.CanPrint) {
		if(modState.ShiftDown) {
			putc(codeSets[1][scancode].UpperASCII);
			isPendingKeyEvent = true;
			pendingKeyEvent = codeSets[1][scancode].UpperASCII;
		} else {
			putc(codeSets[1][scancode].ASCII);
			isPendingKeyEvent = true;
			pendingKeyEvent = codeSets[1][scancode].ASCII;
		}
	}
}

/*void readLine(kchar *str, uint32 length) {
	uint32 i = 0;
	while (i < length-1) {
		kchar last = readChar();
		if (last != '\n') {
			str[i] = last;
		} else {
			break;
		}
		i++;
	}
	str[i+1] = 0; 
}

kchar readChar() {
	isPendingKeyEvent = false;
	while(!isPendingKeyEvent) {

	}
	return pendingKeyEvent;
	isPendingKeyEvent = false;
}
*/
void keyboard_install() {
	irq_install_handler(1, keyboard_handler);
	modState.CtrlDown = false;
	modState.AltDown = false;
	modState.ShiftDown = false;
	modState.CAPSDown = false;
	modState.ESCDown = false;

	codeSets[1][0xAA].IsKeyDown = false;
	codeSets[1][0xAA].IsModif = true;
	codeSets[1][0xAA].ModifNum = SHIFT;
	codeSets[1][0xAA].CanUpper = 0;

	codeSets[1][0xB6].IsKeyDown = false;
	codeSets[1][0xB6].IsModif = true;
	codeSets[1][0xB6].ModifNum = SHIFT;
	codeSets[1][0xB6].CanUpper = 0;
}

void kbd_ack(void) {
	while(!(inportb(0x60)==0xfa));
}
void setKeyboardLEDs(uint8 ledstatus) {
	outportb(0x60,0xed);
	kbd_ack();
	outportb(0x60,ledstatus);
}
