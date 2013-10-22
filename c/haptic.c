#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "timer.h"
#include "pin.h"
#include "usb.h"
#include "oc.h"
#include <stdio.h>
#define SET_SPEED 0
#define GET_SPEED 1
#define GET_EMF 2
#define GET_CURRENT 3
#define SET_CONSTANTS 4
#define GET_TICKCOUNT 5

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

// Constants
uint16_t R1 = 1000;					// For current control, the voltage over this resistor is proportional to current. V = IR
uint16_t k = 0;
uint16_t B = 0;
void __attribute__((interrupt)) _CNInterrupt(void); 
//void countIf(uint16_t);


// Values to be overwritten later
uint16_t tickCount = 0; 			// How many ticks has the encoder read?
uint16_t direction = 0; 			// Encoder direction (1 = clockwise, -1 = counterclockwise)
uint16_t wasLow = 0;				// Encoder value was low, next high will be a new value.
uint16_t encoderSpeed = 0;
uint16_t backEMF = 0;
uint16_t torqueCurrent = 0;
uint16_t torqueOut = 0;
uint16_t speed = 0;
uint16_t invert = 0;

void VendorRequests(void) {
    WORD temp;

    switch (USB_setup.bRequest) {
        case SET_SPEED:
            speed = USB_setup.wValue.w;
            invert = USB_setup.wIndex.w;
            pin_write(pwmPin, speed);
            pin_write(invPin, invert);
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
        case GET_TICKCOUNT:
            temp.w = tickCount;
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

void __attribute__((interrupt, auto_psv)) _CNInterrupt(void) {
    IFS1bits.CNIF = 0;
    //uint16_t encoderVoltage = pin_read(&D[0]);
    //countIf(encoderVoltage);
    tickCount += 1; //direction;
} 

uint16_t getDirection() {
    torqueCurrent = pin_read(fbPin);
    if (torqueCurrent > 0x8000) {
        return 1;
    }
    else if (torqueCurrent < 0x8000) {
        return -1;
    }
    return 0;
    }

void countIf(encoderVoltage) {
    if (encoderVoltage > 0) {
        direction = getDirection();
        tickCount += direction;
    }
    }
/*
interrupt: on falling edge of oc module, read backEMF
void _interruptName (void) {
	interuptFlag = 0; 		//reset flag
	backEMF = sample ADC for backEMF in;
	if (backEMF > 0) {	// direction depends on value of backEMF
		direction = 1;
	}
	else {					//check 
		direction = -1;
	}
}
*/

/*
interrupt: on rising edge of oc module, read torqueCurrent
void _interruptName (void) {
	interruptFlag = 0; 		//reset flag
	uint16_t Vin = sample ADC for voltage proportional to current;
	torqueCurrent = Vin/R1;
}
*/

/*
interrupt: on Encoder rising edge:
void _interruptName (void) {
	resetFlag = 0;
	tickCount = tickCount + direction;
}
*/



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

    // Set up timer1
    timer_setFreq(&timer1, 30);		// timer frequency is 30 Hz
    timer_start(&timer1);



    // Configure motor controller pins
    encoderPin = &D[0];			// encoder in
    pin_digitalIn(encoderPin);
    notSF = &D[1];
    pin_digitalIn(notSF);		// monitor error status
    pwmPin = &D[2];
    pin_digitalOut(pwmPin);
    oc_pwm(&oc1, pwmPin, NULL, 250, (uint16_t)(0));		// write to D1 with a 250Hz PWM signal
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

    while (1) {
        ServiceUSB(); // service any pending USB requests
        /*
        // 
        if (timer_flag(&timer1) == 1) {
            timer_lower(&timer1);
            pin_write(pwmPin, 0);
            backEMF = pin_read(emfPin);
            pin_write(pwmPin, speed);
        }
        */
        /*
        torqueOut = k * tickCount;			// this is scaled completely wrong
        pin_write(pwmPin, torqueOut);
        */
    }
}

