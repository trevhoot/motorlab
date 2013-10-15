#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "timer.h"
#include "pin.h"
#include "usb.h"
#include <stdio.h>
#define SET_MOTOR 0
#define GET_SPEED 1

#define MAX_INT 65535
#define HALF_INT (.5 * 65536)

_PIN * inputPin = 0;
_pin * motorPin = 0;
_pin * directionPin = 0;

uint16_t tickCount = 0; 			// How many ticks has the encoder read?
uint16_t direction = 0; 			// Encoder direction (1 = clockwise, -1 = counterclockwise)
uint16_t wasLow = 0;				// Encoder value was low, next high will be a new value.

void VendorRequests(void) {
    WORD temp;

    switch (USB_setup.bRequest) {
		case SET_MOTOR:
			speed = USB_setup.wValue.w;
			pin_write(motorpin, speed);   //TODO Make this a real thing.
			BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
			break;
		case GET_SPEED:
			temp.w = encoderSpeed;
            BD[EP0IN].address[0] = temp.b[0];
            BD[EP0IN].address[1] = temp.b[1];
            BD[EP0IN].bytecount = 2;    // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
			break;
        default:
            USB_error_flags |= 0x01;    // set Request Error Flag
    }
}

void VendorRequestsIn(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

void VendorRequestsOut(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

int16_t main(void) {
    init_clock();
    init_timer();
    init_pin();

    InitUSB();                              // initialize the USB registers and serial interface engine
    while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
        ServiceUSB();                       // ...service USB requests
    }

	motorPin = &D[0];
	pin_digitalOut(motorPin);    //TODO Change this to Analog, for sure.
	inputPin = &D[1];
	pin_digitalIn(inputPin);
	directionPin = &D[2];
	pin_digitalIn(directionPin);  

    while (1) {
        ServiceUSB(); // service any pending USB requests

        encoderSpeed = pin_read(inputPin);
		direction = pin_read(directionPin);				//what is this value? (is it -1 or 1?)


		if (encoderSpeed == 1 && wasLow == 1) {			// I don't really know how this works. If the encoder reads a new gap
			tickCount = tickCount + direction;		// Add the directionPin value (1 or -1) TODO check the incrementing thing!)
			wasLow = 0;
		}

		if (encoderSpeed != 1) {						// if he encoder reads low, the next high encoder will be a new tick.
			wasLow = 1;
		}

    }
}

