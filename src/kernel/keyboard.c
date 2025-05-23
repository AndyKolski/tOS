#include <ctype.h>
#include <display.h>
#include <interrupts/irq.h>
#include <io.h>
#include <keyboard.h>
#include <stdio.h>
#include <string.h>
#include <system.h>
#include <time/time.h>

#define IRQ_KEYBOARD          1
#define I8042_BUFFER          0x60
#define I8042_STATUS          0x64
#define I8042_ACK             0xFA
#define I8042_BUFFER_FULL     0x01
#define I8042_WHICH_BUFFER    0x20
#define I8042_MOUSE_BUFFER    0x20
#define I8042_KEYBOARD_BUFFER 0x00

typedef struct keyPressEventDataLUTEntry {
	char ASCII;
	char UpperASCII;
	bool CanUppercase;
	bool IsPrintable;
	bool IsModifierKey;
	bool IsOnNumpad;
	bool CapsLockWorks;
	uint8 VKeyCode;
} keyPressEventDataLUTEntry;

typedef struct lockState {
	bool CapsLock;
	bool NumLock;
	bool ScrollLock;
} lockState;

keyPressEvent makeKeyPressEvent(uint8 code, char ASCII) {
	keyPressEvent event = {0};
	event.code = code;
	event.ASCII = ASCII;
	event.isValid = (code == KEY_Invalid ? false : true);
	return event;
}

volatile bool terminalEcho = true;

#define KEYBOARD_BUFFER_SIZE 64

volatile keyPressEvent keyboardBuffer[KEYBOARD_BUFFER_SIZE];
volatile uint32 bufferOffset = 0;

const keyPressEventDataLUTEntry scancodeLookup[255] = {
//    ┌ ASCII form
//    │    ┌ Uppercase ASCII form
//    │    │   ┌ Can this key be uppercased?
//    │    │   │  ┌ Is this key printable?
//    │    │   │  │  ┌ Is this a modifier key?
//    │    │   │  │  │  ┌ Is this key on the numpad?
//    │    │   │  │  │  │  ┌ Is this key affected by Caps Lock?
//    │    │   │  │  │  │  │  ┌ Virtual key code
//    │    │   │  │  │  │  │  │
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x00
	{ 0 ,  0 , 0, 0, 1, 0, 0, KEY_Escape},            // 0x01
	{'1', '!', 1, 1, 0, 0, 0, KEY_1},                 // 0x02
	{'2', '@', 1, 1, 0, 0, 0, KEY_2},                 // 0x03
	{'3', '#', 1, 1, 0, 0, 0, KEY_3},                 // 0x04
	{'4', '$', 1, 1, 0, 0, 0, KEY_4},                 // 0x05
	{'5', '%', 1, 1, 0, 0, 0, KEY_5},                 // 0x06
	{'6', '^', 1, 1, 0, 0, 0, KEY_6},                 // 0x07
	{'7', '&', 1, 1, 0, 0, 0, KEY_7},                 // 0x08
	{'8', '*', 1, 1, 0, 0, 0, KEY_8},                 // 0x09
	{'9', '(', 1, 1, 0, 0, 0, KEY_9},                 // 0x0A
	{'0', ')', 1, 1, 0, 0, 0, KEY_0},                 // 0x0B
	{'-', '_', 1, 1, 0, 0, 0, KEY_Minus},             // 0x0C
	{'=', '+', 1, 1, 0, 0, 0, KEY_Equals},            // 0x0D
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Backspace},         // 0x0E
	{'\t', 0 , 0, 1, 0, 0, 0, KEY_Tab},               // 0x0F
	{'q', 'Q', 1, 1, 0, 0, 1, KEY_Q},                 // 0x10
	{'w', 'W', 1, 1, 0, 0, 1, KEY_W},                 // 0x11
	{'e', 'E', 1, 1, 0, 0, 1, KEY_E},                 // 0x12
	{'r', 'R', 1, 1, 0, 0, 1, KEY_R},                 // 0x13
	{'t', 'T', 1, 1, 0, 0, 1, KEY_T},                 // 0x14
	{'y', 'Y', 1, 1, 0, 0, 1, KEY_Y},                 // 0x15
	{'u', 'U', 1, 1, 0, 0, 1, KEY_U},                 // 0x16
	{'i', 'I', 1, 1, 0, 0, 1, KEY_I},                 // 0x17
	{'o', 'O', 1, 1, 0, 0, 1, KEY_O},                 // 0x18
	{'p', 'P', 1, 1, 0, 0, 1, KEY_P},                 // 0x19
	{'[', '{', 1, 1, 0, 0, 0, KEY_LeftBracket},       // 0x1A
	{']', '}', 1, 1, 0, 0, 0, KEY_RightBracket},      // 0x1B
	{'\n', 0 , 0, 1, 0, 0, 0, KEY_Enter},             // 0x1C
	{ 0 ,  0 , 0, 0, 1, 0, 0, KEY_LeftControl},       // 0x1D
	{'a', 'A', 1, 1, 0, 0, 1, KEY_A},                 // 0x1E
	{'s', 'S', 1, 1, 0, 0, 1, KEY_S},                 // 0x1F
	{'d', 'D', 1, 1, 0, 0, 1, KEY_D},                 // 0x20
	{'f', 'F', 1, 1, 0, 0, 1, KEY_F},                 // 0x21
	{'g', 'G', 1, 1, 0, 0, 1, KEY_G},                 // 0x22
	{'h', 'H', 1, 1, 0, 0, 1, KEY_H},                 // 0x23
	{'j', 'J', 1, 1, 0, 0, 1, KEY_J},                 // 0x24
	{'k', 'K', 1, 1, 0, 0, 1, KEY_K},                 // 0x25
	{'l', 'L', 1, 1, 0, 0, 1, KEY_L},                 // 0x26
	{';', ':', 1, 1, 0, 0, 0, KEY_Semicolon},         // 0x27
	{'\'','"', 1, 1, 0, 0, 0, KEY_Quote},             // 0x28
	{'`', '~', 1, 1, 0, 0, 0, KEY_Grave},             // 0x29
	{ 0 ,  0 , 0, 0, 1, 0, 0, KEY_LeftShift},         // 0x2A
	{'\\','|', 1, 1, 0, 0, 0, KEY_Backslash},         // 0x2B
	{'z', 'Z', 1, 1, 0, 0, 1, KEY_Z},                 // 0x2C
	{'x', 'X', 1, 1, 0, 0, 1, KEY_X},                 // 0x2D
	{'c', 'C', 1, 1, 0, 0, 1, KEY_C},                 // 0x2E
	{'v', 'V', 1, 1, 0, 0, 1, KEY_V},                 // 0x2F
	{'b', 'B', 1, 1, 0, 0, 1, KEY_B},                 // 0x30
	{'n', 'N', 1, 1, 0, 0, 1, KEY_N},                 // 0x31
	{'m', 'M', 1, 1, 0, 0, 1, KEY_M},                 // 0x32
	{',', '<', 1, 1, 0, 0, 0, KEY_Comma},             // 0x33
	{'.', '>', 1, 1, 0, 0, 0, KEY_Period},            // 0x34
	{'/', '?', 1, 1, 0, 0, 0, KEY_Slash},             // 0x35
	{ 0 ,  0 , 0, 0, 1, 0, 0, KEY_RightShift},        // 0x36
	{'*',  0 , 0, 1, 0, 1, 0, KEY_Numpad_Asterisk},   // 0x37
	{ 0 ,  0 , 0, 0, 1, 0, 0, KEY_LeftAlt},           // 0x38
	{' ',  0 , 0, 1, 0, 0, 0, KEY_Space},             // 0x39
	{ 0 ,  0 , 0, 0, 1, 0, 0, KEY_CapsLock},          // 0x3A
	{ 0 ,  0 , 0, 0, 1, 0, 0, KEY_F1},                // 0x3B
	{ 0 ,  0 , 0, 0, 1, 0, 0, KEY_F2},                // 0x3C
	{ 0 ,  0 , 0, 0, 1, 0, 0, KEY_F3},                // 0x3D
	{ 0 ,  0 , 0, 0, 1, 0, 0, KEY_F4},                // 0x3E
	{ 0 ,  0 , 0, 0, 1, 0, 0, KEY_F5},                // 0x3F
	{ 0 ,  0 , 0, 0, 1, 0, 0, KEY_F6},                // 0x40
	{ 0 ,  0 , 0, 0, 1, 0, 0, KEY_F7},                // 0x41
	{ 0 ,  0 , 0, 0, 1, 0, 0, KEY_F8},                // 0x42
	{ 0 ,  0 , 0, 0, 1, 0, 0, KEY_F9},                // 0x43
	{ 0 ,  0 , 0, 0, 1, 0, 0, KEY_F10},               // 0x44
	{ 0 ,  0 , 0, 0, 1, 0, 0, KEY_NumLock},           // 0x45
	{ 0 ,  0 , 0, 0, 1, 0, 0, KEY_ScrollLock},        // 0x46
	{'7',  0 , 0, 1, 0, 1, 0, KEY_Numpad_7},          // 0x47
	{'8',  0 , 0, 1, 0, 1, 0, KEY_Numpad_8},          // 0x48
	{'9',  0 , 0, 1, 0, 1, 0, KEY_Numpad_9},          // 0x49
	{'-',  0 , 0, 1, 0, 1, 0, KEY_Numpad_Minus},      // 0x4A
	{'4',  0 , 0, 1, 0, 1, 0, KEY_Numpad_4},          // 0x4B
	{'5',  0 , 0, 1, 0, 1, 0, KEY_Numpad_5},          // 0x4C
	{'6',  0 , 0, 1, 0, 1, 0, KEY_Numpad_6},          // 0x4D
	{'+',  0 , 0, 1, 0, 1, 0, KEY_Numpad_Plus},       // 0x4E
	{'1',  0 , 0, 1, 0, 1, 0, KEY_Numpad_1},          // 0x4F
	{'2',  0 , 0, 1, 0, 1, 0, KEY_Numpad_2},          // 0x50
	{'3',  0 , 0, 1, 0, 1, 0, KEY_Numpad_3},          // 0x51
	{'0',  0 , 0, 1, 0, 1, 0, KEY_Numpad_0},          // 0x52
	{'.',  0 , 0, 1, 0, 1, 0, KEY_Numpad_Period},     // 0x53
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x54
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x55
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x56
	{ 0 ,  0 , 0, 0, 1, 0, 0, KEY_F11},               // 0x57
	{ 0 ,  0 , 0, 0, 1, 0, 0, KEY_F12},               // 0x58
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x5A
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x59
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x5B
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x5C
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x5D
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x5E
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x5F
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x60
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x61
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x62
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x63
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x64
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x65
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x66
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x67
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x68
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x69
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x6A
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x6B
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x6C
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x6D
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x6E
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x6F
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x70
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x71
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x72
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x73
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x74
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x75
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x76
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x77
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x78
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x79
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x7A
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x7B
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x7C
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x7D
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid},           // 0x7E
	{ 0 ,  0 , 0, 0, 0, 0, 0, KEY_Invalid}            // 0x7F
};
const keyPressEventDataLUTEntry e0ScancodeLookup[255] = {
//    ┌ ASCII form
//    │    ┌ Uppercase ASCII form
//    │    │   ┌ Can this key be uppercased?
//    │    │   │  ┌ Is this key printable?
//    │    │   │  │  ┌ Is this a modifier key?
//    │    │   │  │  │  ┌ Is this key on the numpad?
//    │    │   │  │  │  │  ┌ Is this key affected by Caps Lock?
//    │    │   │  │  │  │  │  ┌ Virtual key code
//    │    │   │  │  │  │  │  │
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x00
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x01
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x02
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x03
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x04
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x05
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x06
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x07
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x08
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x09
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x0A
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x0B
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x0C
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x0D
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x0E
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x0F
	{ 0,   0,  0, 0, 0, 0, 0, KEY_PreviousTrack},     // 0x10
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x11
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x12
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x13
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x14
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x15
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x16
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x17
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x18
	{ 0,   0,  0, 0, 0, 0, 0, KEY_NextTrack},         // 0x19
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x1A
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x1B
	{ 0,   0,  0, 0, 0, 1, 0, KEY_Numpad_Enter},      // 0x1C
	{ 0,   0,  0, 0, 0, 0, 0, KEY_RightControl},      // 0x1D
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x1E
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x1F
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Mute},              // 0x20
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Calculator},        // 0x21
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Play},              // 0x22
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x23
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Stop},              // 0x24
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x25
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x26
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x27
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x28
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x29
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x2A
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x2B
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x2C
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x2D
	{ 0,   0,  0, 0, 0, 0, 0, KEY_VolumeDown},        // 0x2E
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x2F
	{ 0,   0,  0, 0, 0, 0, 0, KEY_VolumeUp},          // 0x30
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x31
	{ 0,   0,  0, 0, 0, 0, 0, KEY_WWW_Home},          // 0x32
	{'/',  0,  0, 1, 0, 1, 0, KEY_Numpad_Slash},      // 0x33
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x34
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x35
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x36
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x37
	{ 0,   0,  0, 0, 0, 0, 0, KEY_RightAlt},          // 0x38
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x39
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x3A
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x3B
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x3C
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x3D
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x3E
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x3F
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x40
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x41
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x42
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x43
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x44
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x45
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x46
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Home},              // 0x47
	{ 0,   0,  0, 0, 0, 0, 0, KEY_UpArrow},           // 0x48
	{ 0,   0,  0, 0, 0, 0, 0, KEY_PageUp},            // 0x49
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x4A
	{ 0,   0,  0, 0, 0, 0, 0, KEY_LeftArrow},         // 0x4B
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x4C
	{ 0,   0,  0, 0, 0, 0, 0, KEY_RightArrow},        // 0x4D
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x4E
	{ 0,   0,  0, 0, 0, 0, 0, KEY_End},               // 0x4F
	{ 0,   0,  0, 0, 0, 0, 0, KEY_DownArrow},         // 0x50
	{ 0,   0,  0, 0, 0, 0, 0, KEY_PageDown},          // 0x51
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Insert},            // 0x52
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Delete},            // 0x53
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x54
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x55
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x56
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x57
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x58
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x59
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x5A
	{ 0,   0,  0, 0, 0, 0, 0, KEY_LeftOS},            // 0x5B
	{ 0,   0,  0, 0, 0, 0, 0, KEY_RightOS},           // 0x5C
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Menu},              // 0x5D
	{ 0,   0,  0, 0, 0, 0, 0, KEY_ACPI_Power},        // 0x5E
	{ 0,   0,  0, 0, 0, 0, 0, KEY_ACPI_Sleep},        // 0x5F
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x60
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x61
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x62
	{ 0,   0,  0, 0, 0, 0, 0, KEY_ACPI_Wake},         // 0x63
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x64
	{ 0,   0,  0, 0, 0, 0, 0, KEY_WWW_Search},        // 0x65
	{ 0,   0,  0, 0, 0, 0, 0, KEY_WWW_Favorites},     // 0x66
	{ 0,   0,  0, 0, 0, 0, 0, KEY_WWW_Refresh},       // 0x67
	{ 0,   0,  0, 0, 0, 0, 0, KEY_WWW_Stop},          // 0x68
	{ 0,   0,  0, 0, 0, 0, 0, KEY_WWW_Forward},      // 0x69
	{ 0,   0,  0, 0, 0, 0, 0, KEY_WWW_Back},          // 0x6A
	{ 0,   0,  0, 0, 0, 0, 0, KEY_MyComputer},        // 0x6B
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Email},             // 0x6C
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x6D
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x6E
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x6F
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x70
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x71
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x72
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x73
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x74
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x75
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x76
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x77
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x78
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x79
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x7A
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x7B
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x7C
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x7D
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid},           // 0x7E
	{ 0,   0,  0, 0, 0, 0, 0, KEY_Invalid}            // 0x7F
};

#define BUFFER_CALC_OFFSET(in) ((in + bufferOffset) % KEYBOARD_BUFFER_SIZE)

void addCharToBuffer(keyPressEvent c) {
	bufferOffset += 1;
	if (bufferOffset == KEYBOARD_BUFFER_SIZE) {
		bufferOffset = 0;
	}
	keyboardBuffer[BUFFER_CALC_OFFSET(KEYBOARD_BUFFER_SIZE - 1)] = c;
}

void clearBuffer() {
	for (uint32 i = 0; i < KEYBOARD_BUFFER_SIZE; ++i) {
		keyboardBuffer[i] = makeKeyPressEvent(KEY_Invalid, 0);
	}
	bufferOffset = 0;
}

keyPressEvent getCharFromBuffer() {
	for (uint32 i = 0; i < KEYBOARD_BUFFER_SIZE; ++i) {
		keyPressEvent read = keyboardBuffer[BUFFER_CALC_OFFSET(i)];
		if (read.isValid == true) {
			keyboardBuffer[BUFFER_CALC_OFFSET(i)] = makeKeyPressEvent(KEY_Invalid, 0);
			return read;
		}
	}
	return makeKeyPressEvent(KEY_Invalid, 0);
}

volatile bool e0Prefix = false;
volatile bool e1Prefix = false;

volatile bool keyPressStates[256] = {false};

volatile lockState lockStates = {false};

volatile uint8 lastIn = 0;

void keyPressed(keyPressEvent event) {
	addCharToBuffer(event);
	if (terminalEcho) {
		putchar(event.ASCII);
	}
}

void keyboard_handler(struct regs *r __attribute__((__unused__))) {
	uint8 status = inb(I8042_STATUS);

	if (!(((status & I8042_WHICH_BUFFER) == I8042_KEYBOARD_BUFFER) && (status & I8042_BUFFER_FULL))) {
		return;
	}
	uint8 inByte = inb(I8042_BUFFER);
	if (inByte == I8042_ACK) {
		return;
	}
	if (inByte == 0xe0) {
		e0Prefix = true;
		return;
	}
	if (inByte == 0xe1) {
		e1Prefix = true;
		return;
	}
	bool isKeyDownEvent = !(inByte & 0x80);
	uint8 scancode = inByte & 0x7f;

	keyPressEventDataLUTEntry keyData = {0};

	if (e0Prefix) {
		if (lastIn == 0x2A && inByte == 0x37) { // Printscreen pressed
			keyData.VKeyCode = KEY_PrintScreen;
			isKeyDownEvent = true;
		} else if (lastIn == 0xB7 && inByte == 0xAA) { // Printscreen released
			keyData.VKeyCode = KEY_PrintScreen;
			isKeyDownEvent = false;
		} else {
			keyData = e0ScancodeLookup[scancode];
		}
	} else if (e1Prefix) {
		if (inByte == 0x1D || inByte == 0x45 || inByte == 0x9D) {
			return;
		} else if (inByte == 0xC5) { // Pause/Break pressed
			e1Prefix = false;
			keyData.VKeyCode = KEY_Break;
			isKeyDownEvent = true;
		} else { // ???
			e1Prefix = false;
			keyData.VKeyCode = KEY_Invalid;
			isKeyDownEvent = true;
			panic("Unexpected E1 code");
		}
	} else {
		keyData = scancodeLookup[scancode];
	}

	lastIn = inByte;

	setKeyDownState(keyData.VKeyCode, isKeyDownEvent);

	if (keyPressStates[KEY_LeftShift] && keyPressStates[KEY_Escape]) {
		reboot();
	}
	if (keyPressStates[KEY_LeftControl] && keyPressStates[KEY_LeftAlt] && keyPressStates[KEY_C]) {
		clearScreen();
	}

	if (keyData.VKeyCode == KEY_Invalid) {
		return;
		// printf("%x (%x) %s %s %i %c\n", scancode, inByte, isKeyDownEvent ? "down" : "up", e0Prefix ? "true" : "false", keyData.VKeyCode, keyData.ASCII);
	}

	if (keyData.VKeyCode == KEY_Break) { // Break doesn't have an up code apparently
		setKeyDownState(KEY_Break, false);
	}

	if (isKeyDownEvent) {
		// printf("Id: %i (%s) ", keyData.VKeyCode, "");

		if (keyData.IsPrintable) {
			if (XOR(lockStates.CapsLock && keyData.CapsLockWorks, keyPressStates[KEY_LeftShift] || keyPressStates[KEY_RightShift]) && keyData.CanUppercase) {
				keyPressed(makeKeyPressEvent(keyData.VKeyCode, keyData.UpperASCII));
			} else {
				keyPressed(makeKeyPressEvent(keyData.VKeyCode, keyData.ASCII));
			}
		} else if (keyData.VKeyCode == KEY_Backspace) {
			keyPressed(makeKeyPressEvent(keyData.VKeyCode, '\b'));
		} else {
			keyPressed(makeKeyPressEvent(keyData.VKeyCode, 0));
		}

		if (keyData.VKeyCode == KEY_CapsLock) {
			lockStates.CapsLock = !lockStates.CapsLock;
		}
		if (keyData.VKeyCode == KEY_NumLock) {
			lockStates.NumLock = !lockStates.NumLock;
		}
		if (keyData.VKeyCode == KEY_ScrollLock) {
			lockStates.ScrollLock = !lockStates.ScrollLock;
		}
		updateLEDs();
	}

	e0Prefix = false;
	return;
}

void keyboardKeyPress(char key) {
	keyPressed(makeKeyPressEvent(KEY_Unknown, key));
}

void updateLEDs() {
	setKeyboardLEDs((lockStates.ScrollLock) | (lockStates.NumLock << 1) | (lockStates.CapsLock << 2));
}

bool isKeyDown(uint8 keyId) {
	return keyPressStates[keyId];
}

void setKeyDownState(uint8 keyId, bool newState) {
	if (newState != keyPressStates[keyId]) { // on state change
	}
	keyPressStates[keyId] = newState;
}

// TODO: Implement a better way for functions (and eventually programs) to
// wait for and read key state changes/presses.

void clearKeyboardBuffer() {
	clearBuffer();
}

char readChar() {
	while (true) {
		keyPressEvent read = getCharFromBuffer();
		if (read.isValid) {
			return read.ASCII;
		}
		wait(1);
	}
}
keyPressEvent readKey() {
	while (true) {
		keyPressEvent read = getCharFromBuffer();
		if (read.isValid) {
			return read;
		}
		wait(1);
	}
}

/// @brief Reads a line of input from the keyboard into the provided buffer.
/// @param buffer The buffer to store the input string.
/// @param bufferCapacity The maximum capacity of the buffer, including space for the null terminator.
/// @param prompt The prompt to display before reading input.
/// @return The length of the input string, excluding the null terminator.
uint32 readLine(char *buffer, uint32 bufferCapacity, const char *prompt) {
	uint32 cursorPos = strlen(buffer);
	printf(prompt);

	bool terminalEchoInitialState = getTerminalEcho();
	setTerminalEcho(false);

	uint32 startingY = getCursorYPosition();
	uint32 startingX = getCursorXPosition();
	while (true) {
		if (cursorPos >= bufferCapacity) {
			cursorPos = bufferCapacity - 1;
		}
		setCursorPosition(startingX, startingY);
		printf("%s ", buffer);

		setCursorPosition(startingX + cursorPos, startingY);
		keyPressEvent event = readKey();

		uint32 bufferStrLen = strlen(buffer);

		if (event.code == KEY_Enter) {
			printf("\n");
			break;
		} else if (event.code == KEY_LeftArrow) {
			if (cursorPos > 0) {
				cursorPos--;
			}
		} else if (event.code == KEY_RightArrow) {
			if (cursorPos < bufferStrLen) {
				cursorPos++;
			}
		} else if (event.code == KEY_Home) {
			cursorPos = 0;
		} else if (event.code == KEY_End) {
			cursorPos = bufferStrLen;
		} else if (event.code == KEY_Delete) {
			if (cursorPos >= bufferStrLen) {
				continue;
			}
			for (size_t i = 0; i < bufferStrLen - cursorPos + 1; i++) {
				buffer[cursorPos + i] = buffer[cursorPos + i + 1];
			}
		} else if (event.code == KEY_Backspace) {
			if (cursorPos == 0) {
				continue;
			}
			for (size_t i = 0; i < bufferStrLen - cursorPos + 1; i++) {
				buffer[cursorPos + i - 1] = buffer[cursorPos + i];
			}

			cursorPos--;

		} else if (isPrint(event.ASCII)) {
			if (bufferStrLen >= bufferCapacity - 1) { // leave room for a final null character
				continue;
			}

			if (cursorPos < bufferStrLen) {
				for (size_t i = 0; i < bufferStrLen - cursorPos; i++) {
					buffer[bufferStrLen - i] = buffer[bufferStrLen - i - 1];
				}
			}

			buffer[cursorPos] = event.ASCII;
			cursorPos++;
		}
	}

	setTerminalEcho(terminalEchoInitialState);

	return strlen(buffer);
}

void setTerminalEcho(bool new) {
	terminalEcho = new;
}

bool getTerminalEcho() {
	return terminalEcho;
}

void initKeyboard() {
	irq_install_handler(1, keyboard_handler);
	updateLEDs();
}

void kbd_ack(void) {
	while (inb(0x60) != 0xfa) {}
}
void setKeyboardLEDs(uint8 ledstatus) {
	outb(0x60, 0xed);
	kbd_ack();
	outb(0x60, ledstatus);
}
