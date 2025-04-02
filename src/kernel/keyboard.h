#include <system.h>

#pragma once

typedef struct keyPressEvent {
	uint8 code;
	char ASCII;
	bool isValid;
} keyPressEvent;

void initKeyboard();
bool isKeyDown(uint8 keyId);
void setKeyDownState(uint8 keyId, bool newState);
void keyboardKeyPress(char key);
void updateLEDs();
void setKeyboardLEDs(uint8 ledstatus);
char readChar();
keyPressEvent readKey();
uint32 readLine(char *buffer, uint32 bufferCapacity, const char *prompt);
void setTerminalEcho(bool new);
bool getTerminalEcho();
void clearKeyboardBuffer();

#define KEY_Unknown 0
#define KEY_Invalid 1

// mouse keys
#define KEY_Mouse_Left 2
#define KEY_Mouse_Right 3
#define KEY_Mouse_Middle 4
#define KEY_Mouse_4 5
#define KEY_Mouse_5 6
#define KEY_Mouse_WheelDown 7
#define KEY_Mouse_WheelUp 8
#define KEY_Mouse_WheelLeft 9
#define KEY_Mouse_WheelRight 10

// letters
#define KEY_A 11
#define KEY_B 12
#define KEY_C 13
#define KEY_D 14
#define KEY_E 15
#define KEY_F 16
#define KEY_G 17
#define KEY_H 18
#define KEY_I 19
#define KEY_J 20
#define KEY_K 21
#define KEY_L 22
#define KEY_M 23
#define KEY_N 24
#define KEY_O 25
#define KEY_P 26
#define KEY_Q 27
#define KEY_R 28
#define KEY_S 29
#define KEY_T 30
#define KEY_U 31
#define KEY_V 32
#define KEY_W 33
#define KEY_X 34
#define KEY_Y 35
#define KEY_Z 36

// number keys
#define KEY_0 37
#define KEY_1 38
#define KEY_2 39
#define KEY_3 40
#define KEY_4 41
#define KEY_5 42
#define KEY_6 43
#define KEY_7 44
#define KEY_8 45
#define KEY_9 46

// numpad numerical keys
#define KEY_Numpad_0 47
#define KEY_Numpad_1 48
#define KEY_Numpad_2 49
#define KEY_Numpad_3 50
#define KEY_Numpad_4 51
#define KEY_Numpad_5 52
#define KEY_Numpad_6 53
#define KEY_Numpad_7 54
#define KEY_Numpad_8 55
#define KEY_Numpad_9 56

// numpad other keys
#define KEY_Numpad_Period 57
#define KEY_Numpad_Slash 58
#define KEY_Numpad_Asterisk 59
#define KEY_Numpad_Plus 60
#define KEY_Numpad_Minus 61
#define KEY_Numpad_Enter 62

// lock keys
#define KEY_NumLock 63
#define KEY_ScrollLock 64
#define KEY_CapsLock 65

// arrow keys
#define KEY_UpArrow 66
#define KEY_DownArrow 67
#define KEY_LeftArrow 68
#define KEY_RightArrow 69

// modifier keys
#define KEY_LeftShift 70
#define KEY_RightShift 71
#define KEY_LeftControl 72
#define KEY_RightControl 73
#define KEY_LeftOS 74
#define KEY_RightOS 75
#define KEY_LeftAlt 76
#define KEY_RightAlt 77

// printable other characters
#define KEY_Space 78
#define KEY_Quote 79
#define KEY_Grave 80
#define KEY_Tab 81
#define KEY_Minus 82
#define KEY_Equals 83
#define KEY_LeftBracket 84
#define KEY_RightBracket 85
#define KEY_Backslash 86
#define KEY_Semicolon 87
#define KEY_Comma 88
#define KEY_Period 89
#define KEY_Slash 90
#define KEY_Enter 91

// non-printable other characters
#define KEY_Backspace 92
#define KEY_Escape 93
#define KEY_Menu 94
#define KEY_Home 95
#define KEY_End 96
#define KEY_PageUp 97
#define KEY_PageDown 98
#define KEY_Insert 99
#define KEY_Break 100
#define KEY_Delete 101
#define KEY_PrintScreen 102

// Fx keys
#define KEY_F1 103
#define KEY_F2 104
#define KEY_F3 105
#define KEY_F4 106
#define KEY_F5 107
#define KEY_F6 108
#define KEY_F7 109
#define KEY_F8 110
#define KEY_F9 111
#define KEY_F10 112
#define KEY_F11 113
#define KEY_F12 114
#define KEY_F13 115
#define KEY_F14 116
#define KEY_F15 117
#define KEY_F16 118
#define KEY_F17 119
#define KEY_F18 120
#define KEY_F19 121
#define KEY_F20 122
#define KEY_F21 123
#define KEY_F22 124
#define KEY_F23 125
#define KEY_F24 126

// multimedia keys
#define KEY_Play 127
#define KEY_Stop 128
#define KEY_PreviousTrack 129
#define KEY_NextTrack 130
#define KEY_VolumeUp 131
#define KEY_VolumeDown 132
#define KEY_Mute 133
#define KEY_Calculator 134
#define KEY_MyComputer 135
#define KEY_Email 136
#define KEY_WWW_Home 137
#define KEY_WWW_Search 138
#define KEY_WWW_Favorites 139
#define KEY_WWW_Refresh 140
#define KEY_WWW_Stop 141
#define KEY_WWW_Forward 142
#define KEY_WWW_Back 143
#define KEY_ACPI_Power 144
#define KEY_ACPI_Sleep 145
#define KEY_ACPI_Wake 146
