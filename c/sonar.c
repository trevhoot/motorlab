#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "timer.h"
#include "pin.h"
#include "oc.h"
#include "usb.h"
#include <stdio.h>
#define MAX_INT 65535
#define SET_VALS 0
#define PING 1
#define GET_TIME 2
#define HALF_INT (.5 * 65536)

//_TIMER * blipTimer = 0;
_PIN * signalOut = 0;
_PIN * inputPin = 0;

uint16_t sensorTime = 0;		// time since start of ping
uint16_t pingTime = 0;			// time that ping is up for
uint16_t overflow = 0;			// has timer1 overflowed?
uint16_t reading = 0; 			// a reading has been made?
uint16_t first = 1; 			// is this the first reading?




uint16_t pulseTime = 3000;
uint16_t listenTime = 8200;


void resetTimer(_TIMER * timer)
{
  timer_stop(timer);
  timer_start(timer);
}
/*
void pingFinished(_TIMER * timer)
{
  pin_write(signalOut, 0);
  timer_stop(timer);
}
*/
void pingFinished(_TIMER * timer)
{
  pin_write(signalOut, 0);
}

void VendorRequests(void) {
    uint16_t pan = 0;
    uint16_t tilt = 0;
    WORD temp;

    switch (USB_setup.bRequest) {
        case SET_VALS:
            pan = USB_setup.wValue.w;
            tilt = USB_setup.wIndex.w;
            pin_write(&D[2], pan);
            pin_write(&D[3], tilt);
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case PING:
			pin_write(signalOut, HALF_INT);
            resetTimer(&timer1);
			pingTime = 0;
            sensorTime = 0;
			overflow = 0;
			reading = 0;
			first = 1;
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case GET_TIME:
            temp.w = sensorTime;
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
    init_oc();

    InitUSB();                              // initialize the USB registers and serial interface engine
    while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
        ServiceUSB();                       // ...service USB requests
    }

    signalOut = &D[4];						// set the digital output and input pins
    pin_digitalOut(signalOut);
    inputPin = &D[1];
    pin_digitalIn(inputPin);

    timer_setFreq(&timer1, 2);		// timer1 period is 500ms

    oc_servo(&oc1, &D[2], &timer1, 20e-3, 5.5e-4, 23.8e-4, (uint16_t)(0.9*65536));	//pan
    oc_servo(&oc2, &D[3], &timer2, 20e-3, 6e-4, 25e-4, (uint16_t)(0.9*65536)); 		//tilt    -->try using the same timer
    oc_pwm(&oc3, signalOut, NULL, 40e3, (uint16_t)(0)); //try using the same timer
    pin_write(signalOut, 0);


    while (1) {
        ServiceUSB(); // service any pending USB requests
        pin_read(inputPin);
        uint16_t input = pin_read(inputPin);
        uint16_t currentTime = timer_read(&timer1);
		overflow = timer_flag(&timer1);

	
		if (currentTime > pulseTime) {
			pin_write(signalOut, 0);
		}
		if (currentTime > listenTime && input == 1) {
			//sensorTime = currentTime;
			reading = 1;
		if (first == 1) {
			sensorTime = currentTime;
			first = 0;
		}
		}

/*
		if (overflow != 0 && reading = 0); {
			sensorTime = 1234;		// if the timer overflows set dummy value 1234
		}
		*/

	/*
        if (lastInput == 0 && input == 1) { //Rising Edge
          uint16_t diffBetweenEdges = currentTime - trailingTime;
          //if (diffBetweenEdges > 50000) {
          if (diffBetweenEdges > 00) {
            //Found a good edge to trigger on
            //sensorTime = currentTime;
          }
          trailingTime = currentTime;
        }
        lastTime = currentTime;
        lastInput = input;
	*/
        /*pin_write(&D[4], (uint16_t) (0 * 65535));
        timer_setFreq(&timer4, 1e1);
        timer_start(&timer4);
        while (!timer_flag(&timer4)) {};
        pin_write(&D[4], (uint16_t) (.5 * 65535));
        timer_setFreq(&timer5, 1e3);
        timer_start(&timer5);
        while (!timer_flag(&timer5)) {};
        */
    }
}

