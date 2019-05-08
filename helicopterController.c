//*****************************************************************************
//
// File: helicopterController.c
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
//
// Main program for the helicopter controller.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "buttons4.h"
#include "circBufT.h"
#include "uartUSB.h"
#include "altitude.h"
#include "yaw.h"
#include "display.h"
#include "scheduler.h"
#include "rotors.h"
#include "control.h"


//*****************************************************************************
// Constants
//*****************************************************************************
#define ALTITUDE_SAMPLE_RATE_HZ        100
#define CONTROL_UPDATE_RATE_HZ         20
#define DISPLAY_UPDATE_RATE_HZ         5
#define BUTTON_CHECK_RATE_HZ           10
#define UART_SEND_RATE_HZ              4

// Altitude sampling is the highest frequency task, so use this as SysTick rate.
#define SYSTICK_RATE_HZ                ALTITUDE_SAMPLE_RATE_HZ

#define ALTITUDE_STEP_PERCENT    10
#define YAW_STEP_DEGREES         15


//*****************************************************************************
// The interrupt handler for the for SysTick interrupt.
//*****************************************************************************
void SysTickIntHandler(void) {
    // Trigger an ADC conversion to measure the current altitude.
    altitudeTriggerConversion();

    updateButtons();
    schedulerUpdateTicks();
}

//*****************************************************************************
// Initialise the clock, setting the rate to 20MHz
//*****************************************************************************
void initClock(void) {
    // Set the clock rate to 20 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
}

//*****************************************************************************
// Configure the SysTick interrupt.
// Should be called after setting the clock rate.
//*****************************************************************************
void initSysTick(void) {
    // Set up the period for the SysTick timer.
    SysTickPeriodSet(SysCtlClockGet() / SYSTICK_RATE_HZ);

    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);

    // Enable the interrupt and the device
    SysTickIntEnable();
    SysTickEnable();
}

//*****************************************************************************
// Checks if the UP or LEFT buttons have been pushed, and if so carries out
// the appropriate actions.
// Note: buttons are updated regularly in the SysTickIntHandler.
//*****************************************************************************
void checkButtons(void) {
    // Cycle the display when the RIGHT button is pushed.
//    if (checkButton(RIGHT) == PUSHED) {
//        displayStateUpdate();
//    }
//
//    // Recalculate the reference sample value when the LEFT button is pushed.
//    if (checkButton(LEFT) == PUSHED) {
//        altitudeResetReference();
//        yawReset();
//    }

    //Experimenting with rotors.
    if (checkButton(RIGHT) == PUSHED) {
        yawChangeDesired(YAW_STEP_DEGREES);
    }

    if (checkButton(LEFT) == PUSHED) {
        yawChangeDesired(-YAW_STEP_DEGREES);
    }

    if (checkButton(UP) == PUSHED) {
        setMainRotorPower(getMainRotorPower() + 5);
    }

    if (checkButton(DOWN) == PUSHED) {
        setMainRotorPower(getMainRotorPower() - 5);
    }

}

int main(void) {
    // Disable interrupts during initialisation.
    IntMasterDisable();

    initClock();
    initSysTick();
    initUart();
    initButtons();
    initDisplay();
    initAltitude();
    initYaw();
    initRotors();
    initControl(CONTROL_UPDATE_RATE_HZ);

    // Initialise the scheduler and register the background tasks with it.
    initScheduler(4);    // Recalculate the reference sample value when the LEFT button is pushed.

    schedulerRegisterTask(controlUpdateYaw,
                          SYSTICK_RATE_HZ / CONTROL_UPDATE_RATE_HZ);
    schedulerRegisterTask(displayUpdate,
                          SYSTICK_RATE_HZ / DISPLAY_UPDATE_RATE_HZ);
    schedulerRegisterTask(uartSendStatus,
                          SYSTICK_RATE_HZ / UART_SEND_RATE_HZ);
    schedulerRegisterTask(checkButtons,
                          SYSTICK_RATE_HZ / BUTTON_CHECK_RATE_HZ);

    // Enable interrupts to the processor once initialisation is complete.
    IntMasterEnable();

    // Set the reference altitude to the current altitude once all other
    // initialisation is complete (and interrupts are enabled).
    altitudeSetInitialReference();

    // Start the rotors, setting their duty cycles to the minimum value.
    startMainRotor();
    startTailRotor();

    // Start running the background tasks.
    schedulerStart();
}
