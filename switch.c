//*****************************************************************************
//
// File: switch.c
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
//
// Module for Switch 1 on the ORBIT daughter board.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "switch.h"


//*****************************************************************************
// Constants
//*****************************************************************************
#define SWITCH_1_PERIPH     SYSCTL_PERIPH_GPIOA
#define SWITCH_1_PORT_BASE  GPIO_PORTA_BASE
#define SWITCH_1_PIN        GPIO_PIN_7


//*****************************************************************************
// Static variables
//*****************************************************************************

// Current position of the switch, true means up.
bool switchPosition;

// Whether the switch position has changed since the last call to checkSwitch1
bool switchPositionChanged = false;


//*****************************************************************************
// Performs initialisation for the main switch.
//*****************************************************************************
void initSwitch() {
    SysCtlPeripheralEnable(SWITCH_1_PERIPH);
    GPIOPinTypeGPIOInput(SWITCH_1_PORT_BASE, SWITCH_1_PIN);
    GPIOPadConfigSet(SWITCH_1_PORT_BASE, SWITCH_1_PIN, GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD_WPD);
    switchPosition = GPIOPinRead(SWITCH_1_PORT_BASE, SWITCH_1_PIN)
                      == SWITCH_1_PIN;
}

//*****************************************************************************
// Checks if the switch position has changed since the last call, and updates
// the switch state as necessary. Should be called frequently, e.g. from a
// SysTickIntHandler.
//*****************************************************************************
void updateSwitch1(void) {
    bool newSwitchPosition = GPIOPinRead(SWITCH_1_PORT_BASE, SWITCH_1_PIN)
                              == SWITCH_1_PIN;
    if (newSwitchPosition != switchPosition) {
        switchPositionChanged = true;
    }
    switchPosition = newSwitchPosition;
}

//*****************************************************************************
// Returns the current state of the switch, indicating whether it has been
// switched up or down, or is unchanged since the last call to this function.
//*****************************************************************************
switchState_t checkSwitch1() {
    if (switchPositionChanged) {
        switchPositionChanged = false;
        if (switchPosition) {
            return SWITCH_UP;
        } else {
            return SWITCH_DOWN;
        }
    } else {
        return SWITCH_UNCHANGED;
    }
}
