//*****************************************************************************
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
#include "altitude.h"
#include "yaw.h"
#include "display.h"


//*****************************************************************************
// Constants
//*****************************************************************************
#define ALTITUDE_SAMPLE_RATE_HZ        400
#define DISPLAY_UPDATE_RATE_HZ         5
#define BUTTON_CHECK_RATE_HZ           10

// Altitude sampling is the highest frequency task, so use this as SysTick rate.
#define SYSTICK_RATE_HZ                ALTITUDE_SAMPLE_RATE_HZ


//*****************************************************************************
// Static variables
//*****************************************************************************
static bool shouldUpdateDisplay = false;  // When true, update display in main loop.
static bool shouldCheckButtons = false;    // When true, check buttons in main loop.


//*****************************************************************************
// The interrupt handler for the for SysTick interrupt.
//*****************************************************************************
void SysTickIntHandler(void) {
    // Trigger an ADC conversion to measure the current altitude.
    altitudeTriggerConversion();

    // Update the buttons.
    updateButtons();

    // Check if it is time to update the display.
    const uint16_t ticksPerDisplayUpdate = SYSTICK_RATE_HZ / DISPLAY_UPDATE_RATE_HZ;
    static uint16_t displayTickCount = 0;
    if (++displayTickCount >= ticksPerDisplayUpdate) {
        displayTickCount = 0;
        shouldUpdateDisplay = true;
    }

    // Check if it is time to handle button changes.
    const uint16_t ticksPerButtonCheck = SYSTICK_RATE_HZ / BUTTON_CHECK_RATE_HZ;
    static uint16_t buttonTickCount = 0;
    if (++buttonTickCount >= ticksPerButtonCheck) {
        buttonTickCount = 0;
        shouldCheckButtons = true;
    }
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
    // Cycle the display when the UP button is pushed.
    if (checkButton(UP) == PUSHED) {
        displayStateUpdate();
    }

    // Recalculate the reference sample value when the LEFT button is pushed.
    if (checkButton(LEFT) == PUSHED) {
        altitudeResetReference();
        yawReset();
    }
}

int main(void) {
    // Disable interrupts during initialisation.
    IntMasterDisable();

    initClock();
    initSysTick();
    initButtons();
    initDisplay();
    initAltitude();
    initYaw();

    // Enable interrupts to the processor once initialisation is complete.
    IntMasterEnable();

    // Set the reference altitude to the current altitude once all other
    // initialisation is complete.
    altitudeSetInitialReference();

    while (1) {
        if (shouldCheckButtons) {
            shouldCheckButtons = false;
            checkButtons();
        }

        if (shouldUpdateDisplay) {
            shouldUpdateDisplay = false;
            displayUpdate(altitudePercent(), altitudeMeanADC(), yawDegrees());
        }
    }
}
