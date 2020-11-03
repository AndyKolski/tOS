#include <display.h>
#include <io.h>
#include <irq.h>
#include <kb.h>
#include <libs.h>
#include <stdio.h>
#include <system.h>

#define IRQ_MOUSE 12
#define I8042_BUFFER 0x60
#define I8042_STATUS 0x64
#define I8042_ACK 0xFA
#define I8042_BUFFER_FULL 0x01
#define I8042_WHICH_BUFFER 0x20
#define I8042_MOUSE_BUFFER 0x20
#define I8042_KEYBOARD_BUFFER 0x00

#define PS2MOUSE_SET_RESOLUTION 0xE8
#define PS2MOUSE_STATUS_REQUEST 0xE9
#define PS2MOUSE_REQUEST_SINGLE_PACKET 0xEB
#define PS2MOUSE_GET_DEVICE_ID 0xF2
#define PS2MOUSE_SET_SAMPLE_RATE 0xF3
#define PS2MOUSE_ENABLE_PACKET_STREAMING 0xF4
#define PS2MOUSE_DISABLE_PACKET_STREAMING 0xF5
#define PS2MOUSE_SET_DEFAULTS 0xF6
#define PS2MOUSE_RESEND 0xFE
#define PS2MOUSE_RESET 0xFF

#define PS2MOUSE_INTELLIMOUSE_ID 0x03
#define PS2MOUSE_INTELLIMOUSE_EXPLORER_ID 0x04

void prepare_for_input() {
    while (true) {
        if (inportb(I8042_STATUS) & 1)
            return;
    }
}
uint8 mouse_read() {
    prepare_for_input();
    return inportb(I8042_BUFFER);
}
uint8 wait_then_read(uint8 port) {
    prepare_for_input();
    return inportb(port);
}
void prepare_for_output() {
    while (true) {
        if (!(inportb(I8042_STATUS) & 2))
            return;
    }
}
void mouse_write(uint8 data) {
    prepare_for_output();
    outportb(I8042_STATUS, 0xd4);
    prepare_for_output();
    outportb(I8042_BUFFER, data);
}
void wait_then_write(uint8 port, uint8 data) {
    prepare_for_output();
    outportb(port, data);
}

void expect_ack() {
    uint8 data = mouse_read();
    if (data != I8042_ACK) {
    	assert("Not ack", false);
    }
}
uint8 get_device_id() {
    mouse_write(PS2MOUSE_GET_DEVICE_ID);
    expect_ack();
    return mouse_read();
}
void set_sample_rate(uint8 rate) {
    mouse_write(PS2MOUSE_SET_SAMPLE_RATE);
    expect_ack();
    mouse_write(rate);
    expect_ack();
}


#define BL 1<<0
#define BR 1<<1
#define BM 1<<2
#define AO 1<<3
#define XS 1<<4
#define YS 1<<5
#define XO 1<<6
#define YO 1<<7

bool mouseIsPresent = false;
bool hasScrollWheel = false;
bool hasFiveButtons = false;

volatile uint8 mouseBuffer[4] = {0};
volatile uint8 mouseBufferPosition = 0;

volatile int32 xPos = 0;
volatile int32 yPos = 0;

volatile int32 zPos = 0;

typedef struct MouseEvent {
	int8 dX;
	int8 dY;
	int8 dZ;
	bool LeftButton;
	bool RightButton;
	bool MiddleButton;
	bool FourthButton;
	bool FifthButton;
} MouseEvent;

void centerMouseOnScreen() {
	xPos = getScreenWidth() / 2;
	yPos = getScreenHeight() / 2;
}

MouseEvent parseMouseData() {
	int8 dX = 0;
	int8 dY = 0;
	int8 dZ = 0;

	MouseEvent event = {0};

	if (hasFiveButtons && hasScrollWheel) {
		dZ = (mouseBuffer[3] & 0x0f);

		if (dZ == 15) {// -1 in 4 bits
		    dZ = -1;
		}
	} else if (hasScrollWheel) { // untested
		dZ = mouseBuffer[3];
	}

	dX = mouseBuffer[1];
	dY = mouseBuffer[2];

	if (dX && mouseBuffer[0] & XS) {
		dX -= 0x100;
	}
	if (dY && mouseBuffer[0] & YS) {
		dY -= 0x100;
	}

	if (mouseBuffer[0] & XO || mouseBuffer[0] & YO) {
		dX = 0;
		dY = 0;
	}

	event.dX = dX;
	event.dY = dY;
	event.dZ = dZ;
	event.LeftButton = (mouseBuffer[0] & BL) == BL;
	event.RightButton = (mouseBuffer[0] & BR) == BR;
	event.MiddleButton = (mouseBuffer[0] & BM) == BM;
	return event;
}

void mouse_handler(struct regs *r __attribute__((__unused__))) {
	uint8 status = inportb(I8042_STATUS);
    if (!(((status & I8042_WHICH_BUFFER) == I8042_MOUSE_BUFFER) && (status & I8042_BUFFER_FULL)))
        return;

    uint8 data = inportb(I8042_BUFFER);
    
    mouseBuffer[mouseBufferPosition] = data;
    MouseEvent event = {0};

    bool isFinishedPacket = false;

    if ((hasScrollWheel || hasFiveButtons) && mouseBufferPosition == 3) {
		event = parseMouseData();
		isFinishedPacket = true;
		mouseBufferPosition = 0;
    } else if (!(hasScrollWheel || hasFiveButtons) && mouseBufferPosition == 2) { // basic mouse
    	event = parseMouseData();
		isFinishedPacket = true;
		mouseBufferPosition = 0;
    } else {
    	mouseBufferPosition++;
    }

    if(isFinishedPacket) {

		// fillRect(xPos, yPos, 5, 5, GColBLACK);
		// fillRect(getScreenWidth()-20, zPos, 20, 10, GColBLACK);

		// xPos += event.dX;
		// yPos -= event.dY;
		// zPos -= event.dZ * 4;

		// if (xPos <= 0) {
		// 	xPos = 0;
		// }
		// if (yPos <= 0) {
		// 	yPos = 0;
		// }
		// if (zPos <= 0) {
		// 	zPos = 0;
		// }
		// if (xPos >= getScreenWidth()) {
		// 	xPos = getScreenWidth();
		// }
		// if (yPos >= getScreenHeight()) {
		// 	yPos = getScreenHeight();
		// }
		// if (zPos + 10 >= getScreenHeight()) {
		// 	zPos = getScreenHeight() - 10;
		// }
		// fillRect(getScreenWidth()-20, zPos, 20, 10, GColBLUE);
		// fillRect(xPos, yPos, 5, 5, GColWHITE);

		setKeyDownState(KEY_Mouse_Left, event.LeftButton);
		setKeyDownState(KEY_Mouse_Right, event.RightButton);
		setKeyDownState(KEY_Mouse_Middle, event.MiddleButton);
		setKeyDownState(KEY_Mouse_4, event.FourthButton);
		setKeyDownState(KEY_Mouse_5, event.FifthButton);
		if (event.LeftButton) {
			printf("Left Click\n");
		}
		if (event.RightButton) {
			printf("Right Click\n");
		}
		if (event.MiddleButton) {
			printf("Middle Click\n");
		}
    }
	return;
}
void mouse_install() {
	centerMouseOnScreen();
    wait_then_write(I8042_STATUS, 0xa8);
    mouse_write(PS2MOUSE_REQUEST_SINGLE_PACKET);
    uint8 maybe_ack = mouse_read();
    if (maybe_ack == I8042_ACK) {
        puts("Mouse detected\n");
    	//Mouse is available. Ignore next 3 packets
        mouse_read();
        mouse_read();
        mouse_read();

        mouseIsPresent = true;

        wait_then_write(I8042_STATUS, 0x20); // get config data
        uint8 status = wait_then_read(I8042_BUFFER);

        wait_then_write(I8042_STATUS, 0x60);
        wait_then_write(I8042_BUFFER, status | 3); // enable mouse & keyboard ints

         // Set default settings.
	    mouse_write(PS2MOUSE_SET_DEFAULTS);
	    expect_ack();

	    // Enable.
	    mouse_write(PS2MOUSE_ENABLE_PACKET_STREAMING);
	    expect_ack();

	    uint8 device_id = get_device_id();
	    if (device_id != PS2MOUSE_INTELLIMOUSE_ID) {
            // Send magical wheel initiation sequence.
            set_sample_rate(200);
            set_sample_rate(100);
            set_sample_rate(80);
            device_id = get_device_id();
        }
        if (device_id == PS2MOUSE_INTELLIMOUSE_ID) {
           hasScrollWheel = true;
           puts("Mouse wheel enabled\n");
       } else {
           puts("No mouse wheel detected\n");
       }

	    if (device_id == PS2MOUSE_INTELLIMOUSE_ID) {
	        // Try to enable 5 buttons as well!
	        set_sample_rate(200);
	        set_sample_rate(200);
	        set_sample_rate(80);
	        device_id = get_device_id();
	    }
	    if (device_id == PS2MOUSE_INTELLIMOUSE_EXPLORER_ID) {
           hasFiveButtons = true;
           puts("5 button mouse mode enabled\n");
       	}
		irq_install_handler(12, mouse_handler);
    } else {
    	puts("No mouse present or detected\n");
    }
}