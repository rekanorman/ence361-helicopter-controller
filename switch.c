//*****************************************************************************
//
// File: switch.c
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
//
// Module for Switch 1 of the on the ORBIT daughter board.
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
#define SWITCH_1_PERIPH  SYSCTL_PERIPH_GPIOA
#define SWITCH_1_PORT_BASE  GPIO_PORTA_BASE
#define SWITCH_1_PIN  GPIO_PIN_7

//*****************************************************************************
// Performs initialisation for the main switch.
//*****************************************************************************
void initSwitch() {
    SysCtlPeripheralEnable(SWITCH_1_PERIPH);
        GPIOPinTypeGPIOInput(SWITCH_1_PORT_BASE, SWITCH_1_PIN);
        GPIOPadConfigSet(SWITCH_1_PORT_BASE, SWITCH_1_PIN, GPIO_STRENGTH_2MA,
                         GPIO_PIN_TYPE_STD_WPD);
}

//*****************************************************************************
// Checks the current state of state of SW1.
//
// returns: The current state of SW1, either SWITCH_UP or SWITCH_DOWN.
//*****************************************************************************
switch_state_t checkSwitch1() {
    if (GPIOPinRead(SWITCH_1_PORT_BASE, SWITCH_1_PIN) == SWITCH_1_PIN) {
        return SWITCH_UP;
    } else {
        return SWITCH_DOWN;
    }
}
