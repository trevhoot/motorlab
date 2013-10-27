#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "timer.h"
#include "pin.h"
#include "usb.h"
#include "oc.h"
#include <stdio.h>
#include <time.h>
#define SET_SPEED 0
#define GET_TICKS 1
#define GET_EMF 2
#define GET_CURRENT 3
#define SET_CONSTANTS 4

#define MAX_INT 65535
#define HALF_INT (.5 * 65536)

// Name motor controller pins
_PIN * encoderPin = 0;
_PIN * pwmPin = 0;
_PIN * notSF = 0;
_PIN * notD2 = 0;
_PIN * D1 = 0;
_PIN * enable = 0;
_PIN * in1 = 0;
_PIN * in2 = 0;
_PIN * invPin = 0;
_PIN * slew = 0;
_PIN * currentPin = 0;
_PIN * emfPin = 0;
_PIN * fbPin = 0;
_PIN * testPin1 = 0;
_PIN * testPin2 = 0;

// Constants
uint16_t R1 = 1000;					// For current control, the voltage over this resistor is proportional to current. V = IR
uint16_t k = 0;
uint16_t B = 0;
void __attribute__((interrupt)) _CNInterrupt(void);
void __attribute__((interrupt)) _OC1Interrupt(void);


// Values to be overwritten later
uint16_t tickCount = 0; 			// How many ticks has the encoder read?
uint16_t direction = 0; 			// Encoder direction (1 = clockwise, -1 = counterclockwise)
uint16_t backEMF = 0;
uint16_t torqueCurrent = 0;
uint16_t torqueOut = 0;
uint16_t duty = 0;
uint16_t invert = 0;
uint16_t PWMstate = 0;
uint16_t delay = 0;
uint16_t ocFreq = 0;

int i = 0;
int var = 1;

void __attribute__((interrupt, auto_psv)) _CNInterrupt(void) {
    IFS1bits.CNIF = 0;
    tickCount += direction;
} 

void readCurrent(void){
    pin_write(testPin1,1);
    torqueCurrent = pin_read(currentPin);
}

void readEMF(void){
    pin_write(testPin1,0);
    backEMF = pin_read(emfPin);
    if (backEMF > 32768){
	direction = -1;
    }
    else {
	direction = 1;
    }
}

float dutytodelay(duty) {
    return duty/65355/ocFreq;
    }



void timer_serviceInterrupt(_TIMER *self) {
    timer_lower(self);
    if (PWMstate == 1) {
        readCurrent();
	PWMstate = 0;
	//var = 1000*dutytodelay(duty);
	var = 500;
	timer_setPeriod(&timer1, 0.004);
	timer_start(self);
    } else {
        readEMF();
	timer_stop(self);
    }
}


void __attribute__((interrupt, auto_psv)) _OC1Interrupt(void) {
    IFS0bits.OC1IF = 0; 		//reset oc flag
    timer_setPeriod(&timer1, 0.001);
    timer_start(&timer1);		// start timer to go off before the fall
    PWMstate = 1;			// set global variable to high
    }

void VendorRequests(void) {
    WORD temp;

    switch (USB_setup.bRequest) {
        case SET_SPEED:
            duty = USB_setup.wValue.w;
            invert = USB_setup.wIndex.w;
            pin_write(pwmPin, duty);
            pin_write(invPin, invert);
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case GET_TICKS:
            temp.w = var;
            BD[EP0IN].address[0] = temp.b[0];
            BD[EP0IN].address[1] = temp.b[1];
            BD[EP0IN].bytecount = 2;    // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break; 
        case GET_EMF:
            temp.w = backEMF;
            BD[EP0IN].address[0] = temp.b[0];
            BD[EP0IN].address[1] = temp.b[1];
            BD[EP0IN].bytecount = 2;    // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;        
        case GET_CURRENT:
            temp.w = torqueCurrent;
            BD[EP0IN].address[0] = temp.b[0];
            BD[EP0IN].address[1] = temp.b[1];
            BD[EP0IN].bytecount = 2;    // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case SET_CONSTANTS:
            k = USB_setup.wValue.w;
            B = USB_setup.wIndex.w;
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0
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

    // Configure Interrupts on the pic
    IEC1bits.CNIE = 1;
    CNEN1bits.CN14IE = 1;
    IFS1bits.CNIF = 0;
    IEC0bits.OC1IE = 1;
    IFS0bits.OC1IF = 0;

    // Configure motor controller pins
    encoderPin = &D[0];			// encoder in
    pin_digitalIn(encoderPin);
    notSF = &D[1];
    pin_digitalIn(notSF);		// monitor error status
    pwmPin = &D[2];
    pin_digitalOut(pwmPin);

    ocFreq = 250;
    oc_pwm(&oc1, pwmPin, NULL, ocFreq, (uint16_t)(0));		// write to D1 with a 250Hz PWM signal
    D1 = &D[3];
    pin_digitalOut(D1);
    pin_write(D1, 0); 		// default low
    enable = &D[4];
    pin_digitalOut(enable);
    pin_write(enable, 1);		// wake up the motor driver
    in1 = &D[5];
    pin_digitalOut(in1);		// set in1 and in2 to different values
    pin_write(in1, 0);
    in2 = &D[6];
    pin_digitalOut(in2);
    pin_write(in1, 1);
    slew = &D[7];
    pin_write(slew, 1);
    invPin = &D[8];				// control the direction of the motor	pin_digitalOut(invPin);

    currentPin = &A[0];
    pin_analogIn(currentPin);
    emfPin = &A[1];
    pin_analogIn(emfPin);
    fbPin = &A[2];
    pin_analogIn(fbPin);

    testPin1 = &D[10];
    pin_digitalOut(testPin1);

    while (1) {
        ServiceUSB(); // service any pending USB requests
        
/*
	torqueOut = k * tickCount		// 96 ticks per rotation, gear ratio is 41.6:1, so about 4000 ticks per rotation. 
						// torqueOut should be between 0 and 65355 (duty MAX_INT)

	if (torqueOut > MAX_INT){
		torqueOut = MAX_INT
	}
*/      

    }
}

