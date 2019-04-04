//*****************************************************************************
//
// ADCdemo1.c - Simple interrupt driven program which samples with AIN9.
//
// This version sends the ADC status across a serial link, using funcions
// in the uartUSB module, as well as displaying it on the OLED display.
//
// Author:  P.J. Bones  UCECE
// Last modified:   8.2.2018
//
//*****************************************************************************
// Based on the 'convert' series from 2016
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "buttons4.h"
#include "circBufT.h"
#include "altitude.h"
#include "display.h"

//*****************************************************************************
// Constants
//*****************************************************************************
#define SAMPLE_RATE_HZ          400
#define DISPLAY_UPDATE_RATE_HZ  5


//*****************************************************************************
// Global variables
//*****************************************************************************
static uint8_t displayUpdateTick = false;  // When true, should update display.
static uint16_t yawChange = 0;         // Change in yaw since last angle update.


//*****************************************************************************
// The interrupt handler for the for SysTick interrupt.
//*****************************************************************************
void SysTickIntHandler(void) {
    // Trigger an ADC conversion to measure the current altitude.
    altitudeTriggerConversion();

    // Update the buttons.
    updateButtons();

    // Check if it is time to update the display.
    const uint16_t ticksPerUpdate = SAMPLE_RATE_HZ / DISPLAY_UPDATE_RATE_HZ;
    static uint16_t tickCount = 0;
    if (++tickCount >= ticksPerUpdate) {
        tickCount = 0;
        displayUpdateTick = true;
    }
}

//*****************************************************************************
//
// The interrupt handler for the for pin change interrupt for port B.
//
//*****************************************************************************
void portBIntHandler(void)
{
    static bool pin0 = false;
    static bool pin1 = false;

    bool nextPin0 = GPIOPinRead (GPIO_PORTB_BASE, GPIO_INT_PIN_0) == GPIO_INT_PIN_0;
    bool nextPin1 = GPIOPinRead (GPIO_PORTB_BASE, GPIO_INT_PIN_1) == GPIO_INT_PIN_1;

    if (pin0 && pin1) {
        if (nextPin0 && !nextPin1) {
            yawChange += 1;
        } else if (nextPin1 && !nextPin0) {
            yawChange -= 1;
        }

    }

    pin0 = nextPin0;
    pin1 = nextPin1;

    GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

}

//***************************************** n************************************
// Initialisation functions for the clock (incl. SysTick)
//*****************************************************************************
void initClock(void) {
    // Set the clock rate to 20 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                    SYSCTL_XTAL_16MHZ);

    // Set up the period for the SysTick timer.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);

    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);

    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}

void initPinChangeInt(void) {

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    GPIOIntRegister(GPIO_PORTB_BASE, portBIntHandler);

    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1, GPIO_BOTH_EDGES);

    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);
}


//*****************************************************************************
// Checks if the UP or LEFT buttons have been pushed, and if so carries out
// the appropriate actions.
// Note: buttons are updated regularly in the SysTickIntHandler.
//*****************************************************************************
void checkButtons(void) {
    // Cycle the display when the UP button is pushed.
    if (checkButton(UP) == PUSHED) {
        displayStateUpdate();
    }

    // Recalculate the reference sample value when the LEFT button is pushed.
    if (checkButton(LEFT) == PUSHED) {
        altitudeResetReference();
    }
}

int main(void) {
    // Disable interrupts during initialisation.
    IntMasterDisable();

    initClock();
    initPinChangeInt();
    initDisplay();
    initButtons();
    initAltitude();

    // Enable interrupts to the processor once initialisation is complete.
    IntMasterEnable();

    altitudeSetInitialReference();

    while (1) {
        checkButtons();
        if (displayUpdateTick) {
            displayUpdateTick = false;
            displayUpdate(altitudePercent(), altitudeMeanADC(), yawChange);
        }
//        if (yawChange) {
//            yawAngle = (yawChange * 3.21);
//            yawChange = 0;
//        }
    }
}

