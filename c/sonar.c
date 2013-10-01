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

_TIMER * blipTimer = 0;
_PIN * signalOut = 0;

void pingFinished(_TIMER * timer)
{
  pin_write(&D[4], 0);
  timer_stop(timer);
}

void VendorRequests(void)
{
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
            pin_write(&D[4], MAX_INT);
            timer_start(&timer4);
            timer_after(&timer4, 0.0f, 1, pingFinished);
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case GET_TIME:
            temp.w = sensorTime;
            BD[EPOIN].address[0] = temp[0];
            BD[EPOIN].address[1] = temp[1];
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

    signalOut = &D[4];
    blipTimer = &timer4;

    setFreq(blipTimer, 1e1);
    //oc_servo(&oc1, &D[2], &timer1, 20e-3, 5.5e-4, 23.8e-4, (uint16_t)(0.9*65536));
    //oc_servo(&oc2, &D[3], &timer2, 20e-3, 6e-4, 25e-4, (uint16_t)(0.9*65536)); //try using the same timer
    oc_pwm(&oc2, signalOut, NULL, 40e3, (uint16_t)(.5f * 65536)); //try using the same timer


    //InitUSB();                              // initialize the USB registers and serial interface engine
    //while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
        //ServiceUSB();                       // ...service USB requests
    //}
    while (1) {
        ServiceUSB();                       // service any pending USB requests
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

    while(1) {}
}

