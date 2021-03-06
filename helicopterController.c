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
#include "buttons4.h"
#include "switch.h"
#include "circBufT.h"
#include "uartUSB.h"
#include "altitude.h"
#include "yaw.h"
#include "display.h"
#include "scheduler.h"
#include "rotors.h"
#include "control.h"
#include "flightState.h"


//*****************************************************************************
// Constants
//*****************************************************************************
#define ALTITUDE_SAMPLE_RATE_HZ            400
#define CONTROL_UPDATE_RATE_HZ             20
#define BUTTON_CHECK_RATE_HZ               10
#define SWITCH_CHECK_RATE_HZ               10
#define DISPLAY_UPDATE_RATE_HZ             5
#define UART_SEND_RATE_HZ                  4
#define UPDATE_TAKEOFF_LANDING_RATE_HZ     2

// Altitude sampling is the highest frequency task, so use this as SysTick rate.
#define SYSTICK_RATE_HZ          ALTITUDE_SAMPLE_RATE_HZ

// The amount by which altitude and yaw change when the buttons are pushed.
#define ALTITUDE_STEP_PERCENT    10
#define YAW_STEP_DEGREES         15


//*****************************************************************************
// The interrupt handler for the for SysTick interrupt.
//*****************************************************************************
void SysTickIntHandler(void) {
    altitudeTriggerConversion();
    updateButtons();
    updateSwitch1();
    schedulerUpdateTicks();
}

//*****************************************************************************
// Initialise the clock, setting the rate to 20MHz
//*****************************************************************************
void initClock(void) {
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

    // Register the interrupt handler.
    SysTickIntRegister(SysTickIntHandler);

    // Enable the interrupt and the device.
    SysTickIntEnable();
    SysTickEnable();
}

//*****************************************************************************
// Checks if any of the buttons have been pushed, and updates the desired
// altitude and yaw as needed.
// Note: buttons are updated regularly in the SysTickIntHandler.
//*****************************************************************************
void checkButtons(void) {
    // The altitude and yaw should only be changed if the helicopter is flying.
    if (getFlightState() == FLYING) {
        if (checkButton(RIGHT) == PUSHED) {
            yawChangeDesired(YAW_STEP_DEGREES);
        }

        if (checkButton(LEFT) == PUSHED) {
            yawChangeDesired(-YAW_STEP_DEGREES);
        }

        if (checkButton(UP) == PUSHED) {
            altitudeChangeDesired(ALTITUDE_STEP_PERCENT);
        }

        if (checkButton(DOWN) == PUSHED) {
            altitudeChangeDesired(-ALTITUDE_STEP_PERCENT);
        }
    }
}

//*****************************************************************************
// Checks the state of the switch and performs the appropriate actions to
// change the state of the helicopter.
//*****************************************************************************
void checkSwitch(void) {
    switchState_t switchState = checkSwitch1();

    if (switchState == SWITCH_UP && getFlightState() == LANDED) {
        startMainRotor();
        startTailRotor();
        setFlightState(FINDING_YAW_REFERENCE);

    } else if (switchState == SWITCH_DOWN && getFlightState() == FLYING) {
        setFlightState(LANDING_YAW);
    }
}

//*****************************************************************************
// If the helicopter is currently taking off (finding the reference yaw signal)
// or landing, updates the desired yaw or altitude, or changes the flight
// state of the helicopter as necessary.
//*****************************************************************************
void updateTakeOffOrLanding(void) {
    if (getFlightState() == FINDING_YAW_REFERENCE) {
        yawChangeDesired(YAW_STEP_DEGREES);
    } else if (getFlightState() == LANDING_YAW) {
        yawUpdateLanding(YAW_STEP_DEGREES);
    } else if (getFlightState() == LANDING_ALTITUDE) {
        altitudeUpdateLanding(ALTITUDE_STEP_PERCENT);
    }
}

int main(void) {
    // Disable interrupts during initialisation.
    IntMasterDisable();

    initClock();
    initSysTick();
    initUart();
    initButtons();
    initSwitch();
    initDisplay();
    initAltitude();
    initYaw();
    initRotors();
    initControl(CONTROL_UPDATE_RATE_HZ);

    setFlightState(LANDED);

    // Initialise the scheduler and register the background tasks with it.
    // Tasks are registered in order of priority, with highest first.
    initScheduler(6);
    schedulerRegisterTask(controlUpdate,
                          SYSTICK_RATE_HZ / CONTROL_UPDATE_RATE_HZ);
    schedulerRegisterTask(checkButtons,
                          SYSTICK_RATE_HZ / BUTTON_CHECK_RATE_HZ);
    schedulerRegisterTask(checkSwitch,
                          SYSTICK_RATE_HZ / SWITCH_CHECK_RATE_HZ);
    schedulerRegisterTask(updateTakeOffOrLanding,
                          SYSTICK_RATE_HZ / UPDATE_TAKEOFF_LANDING_RATE_HZ);
    schedulerRegisterTask(displayUpdate,
                          SYSTICK_RATE_HZ / DISPLAY_UPDATE_RATE_HZ);
    schedulerRegisterTask(uartSendStatus,
                          SYSTICK_RATE_HZ / UART_SEND_RATE_HZ);

    // Enable interrupts to the processor once initialisation is complete.
    IntMasterEnable();

    // Set the reference altitude to the current altitude once all other
    // initialisation is complete (and interrupts are enabled).
    altitudeSetReference();

    // Start running the background tasks.
    schedulerStart();
}
