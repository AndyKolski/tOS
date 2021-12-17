#include <display.h>
#include <io.h>
#include <irq.h>
#include <keyboard.h>
#include <stdio.h>
#include <string.h>
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
        if (inb(I8042_STATUS) & 1)
            return;
    }
}
uint8 mouse_read() {
    prepare_for_input();
    return inb(I8042_BUFFER);
}
uint8 wait_then_read(uint8 port) {
    prepare_for_input();
    return inb(port);
}
void prepare_for_output() {
    while (true) {
        if (!(inb(I8042_STATUS) & 2))
            return;
    }
}
void mouse_write(uint8 data) {
    prepare_for_output();
    outb(I8042_STATUS, 0xd4);
    prepare_for_output();
    outb(I8042_BUFFER, data);
}
void wait_then_write(uint8 port, uint8 data) {
    prepare_for_output();
    outb(port, data);
}

void expect_ack(char* location) {
    char message[64] = "Not ack in ";
    assert(strlen(message) + strlen(location) + 1 <= (int32) sizeof(message), "location string too long");
    strcat(message, location);

    uint8 data = mouse_read();
   	assert(data == I8042_ACK, message);
}
uint8 get_device_id() {
    mouse_write(PS2MOUSE_GET_DEVICE_ID);
    expect_ack("get_device_id");
    return mouse_read();
}
void set_sample_rate(uint8 rate) {
    mouse_write(PS2MOUSE_SET_SAMPLE_RATE);
    expect_ack("get_sample_rate 1");
    mouse_write(rate);
    expect_ack("get_sample_rate 2");
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

// void centerMouseOnScreen() {
// 	xPos = getScreenWidth() / 2;
// 	yPos = getScreenHeight() / 2;
// }

MouseEvent parseMouseData() {
	MouseEvent event = {0};

	event.dX = 0;
	event.dY = 0;
	event.dZ = 0;
	event.LeftButton = (mouseBuffer[0] & BL) == BL;
	event.RightButton = (mouseBuffer[0] & BR) == BR;
	event.MiddleButton = (mouseBuffer[0] & BM) == BM;
	event.FourthButton = false;
	event.FifthButton = false;

	if (hasFiveButtons && hasScrollWheel) {
		event.dZ = (mouseBuffer[3] & 0x0f);
		if (event.dZ == 15) {// -1 in 4 bits
		    event.dZ = -1;
		}
		event.FourthButton = (mouseBuffer[3] & 0x10) == 0x10;
		event.FifthButton = (mouseBuffer[3] & 0x20) == 0x20;
	} else if (hasScrollWheel) {
		event.dZ = mouseBuffer[3] & 0x0f;
	}

	event.dX = (int8)mouseBuffer[1];
	event.dY = (int8)mouseBuffer[2];

	if (event.dX && mouseBuffer[0] & XS) {
		event.dX -= 0x100;
	}
	if (event.dY && mouseBuffer[0] & YS) {
		event.dY -= 0x100;
	}

	if (mouseBuffer[0] & XO || mouseBuffer[0] & YO) {
		event.dX = 0;
		event.dY = 0;
	}
	return event;
}

void mouse_handler(struct regs *r __attribute__((__unused__))) {
	uint8 status = inb(I8042_STATUS);
    if (!(((status & I8042_WHICH_BUFFER) == I8042_MOUSE_BUFFER) && (status & I8042_BUFFER_FULL)))
        return;

    uint8 data = inb(I8042_BUFFER);
    
    mouseBuffer[mouseBufferPosition] = data;
    MouseEvent event = {0};

    bool isFinishedPacket = false;

    if ((hasScrollWheel || hasFiveButtons) && mouseBufferPosition == 3) { // either scroll wheel or 5-button mouse, both use 4-byte packets
		event = parseMouseData();
		isFinishedPacket = true;
		mouseBufferPosition = 0;
    } else if (!(hasScrollWheel || hasFiveButtons) && mouseBufferPosition == 2) { // basic mouse, uses 3-byte packets
    	event = parseMouseData();
		isFinishedPacket = true;
		mouseBufferPosition = 0;
    } else { // buffer not yet full. Wait for next packet
    	mouseBufferPosition++;
    }

    if(isFinishedPacket) {

		// fillRect(xPos, yPos, 5, 5, GColBLACK);
		// fillRect(getScreenWidth()-20, zPos, 20, 10, GColBLACK);

		// xPos += event.dX;
		// yPos -= event.dY;
		// zPos += event.dZ * 8;

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
		if (event.FourthButton) {
			printf("Button 4 Click\n");
		}
		if (event.FifthButton) {
			printf("Button 5 Click\n");
		}
    }
	return;
}
void mouse_install() {
	// centerMouseOnScreen();
    wait_then_write(I8042_STATUS, 0xa8);
    mouse_write(PS2MOUSE_REQUEST_SINGLE_PACKET);
    uint8 maybe_ack = mouse_read();
    if (maybe_ack == I8042_ACK) {
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
	    expect_ack("mouse_install set defaults");

	    // Enable.
	    mouse_write(PS2MOUSE_ENABLE_PACKET_STREAMING);
	    expect_ack("mouse_install enable");

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
       	}
		irq_install_handler(12, mouse_handler);
    	printf("Detected mouse configuration - scroll wheel: %s, buttons: %i\n", (hasScrollWheel ? "true" : "false"), (hasFiveButtons ? 5 : 3));
    } else {
    	puts("No mouse present or detected");
    }
}