//*****************************************************************************
//
// File: yaw.c
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
//
// Module measuring the yaw angle using pin change interrupts.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "yaw.h"


//*****************************************************************************
// Constants
//*****************************************************************************
#define YAW_GPIO_PERIPH         SYSCTL_PERIPH_GPIOB
#define YAW_GPIO_BASE           GPIO_PORTB_BASE
#define YAW_CHANNEL_A_PIN       GPIO_PIN_0
#define YAW_CHANNEL_B_PIN       GPIO_PIN_1
#define YAW_INTERRUPT_TRIGGER   GPIO_BOTH_EDGES   // Detect rising and falling edges

#define NUM_SLOTS_IN_CIRCLE     112
#define NUM_DEGREES_IN_CIRCLE   360


//*****************************************************************************
// Static variables
//*****************************************************************************

// Yaw value relative to the position when the program started, measured
// in the number of slots passed.
static int16_t yawChange = 0;


//*****************************************************************************
// Static function forward declarations.
//*****************************************************************************
static void portBIntHandler(void);


//*****************************************************************************
// Performs initialisation for measuring the yaw.
//*****************************************************************************
void initYaw(void) {
    // Configure the GPIO pins used for measuring the yaw signals.
    SysCtlPeripheralEnable(YAW_GPIO_PERIPH);
    GPIOPinTypeGPIOInput(YAW_GPIO_BASE, YAW_CHANNEL_A_PIN | YAW_CHANNEL_B_PIN);

    // Configure the pin change interrupt to be called whenever the signal on
    // either channel A or channel B changes level.
    GPIOIntRegister(YAW_GPIO_BASE, portBIntHandler);

    GPIOIntTypeSet(YAW_GPIO_BASE,
                   YAW_CHANNEL_A_PIN | YAW_CHANNEL_B_PIN,
                   YAW_INTERRUPT_TRIGGER);

    GPIOIntEnable(YAW_GPIO_BASE, YAW_CHANNEL_A_PIN | YAW_CHANNEL_B_PIN);
}

//*****************************************************************************
// Reset the yaw value to zero.
//*****************************************************************************
void yawReset(void) {
    yawChange = 0;
}

//*****************************************************************************
// The interrupt handler for the for pin change interrupt on port B.
// Compares the current values of the two input yaw channels to their
// previous values and updates the relative yaw if necessary.
//*****************************************************************************
static void portBIntHandler(void) {
    // Variables to keep track of whether each yaw channel was HIGH the last
    // time this interrupt occurred.
    static bool previousChannelA = false;
    static bool previousChannelB = false;

    // Check whether each yaw channel is currently HIGH.
    bool currentChannelA = GPIOPinRead(YAW_GPIO_BASE, YAW_CHANNEL_A_PIN)
                           == YAW_CHANNEL_A_PIN;
    bool currentChannelB = GPIOPinRead(YAW_GPIO_BASE, YAW_CHANNEL_B_PIN)
                           == YAW_CHANNEL_B_PIN;

    // Update the yaw if both channels were previously HIGH.
    if (previousChannelA && previousChannelB) {
        if (currentChannelA && !currentChannelB) {
            // Clockwise rotation
            yawChange += 1;
        } else if (currentChannelB && !currentChannelA) {
            // Anticlockwise rotation
            yawChange -= 1;
        }
    }

    previousChannelA = currentChannelA;
    previousChannelB = currentChannelB;

    GPIOIntClear(YAW_GPIO_BASE, YAW_CHANNEL_A_PIN | YAW_CHANNEL_B_PIN);
}

//*****************************************************************************
// Calculate and return the yaw in degrees, relative to the position when the
// program started. The returned value will be between -180 and 179 inclusive.
//*****************************************************************************
int16_t yawDegrees(void) {
    // Convert yawChange to degrees.
    int16_t degrees = yawChange * NUM_DEGREES_IN_CIRCLE / NUM_SLOTS_IN_CIRCLE;
    // Find remainder when divided by the number of degrees in a circle.
    degrees = degrees % NUM_DEGREES_IN_CIRCLE;

    // Ensure the returned value is between -180 and 179.
    if (degrees < -NUM_DEGREES_IN_CIRCLE / 2) {
        degrees += NUM_DEGREES_IN_CIRCLE;
    } else if (degrees >= NUM_DEGREES_IN_CIRCLE / 2) {
        degrees -= NUM_DEGREES_IN_CIRCLE;
    }
    return degrees;
}

