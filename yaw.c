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
#include "flightState.h"
#include "altitude.h"

#include "yaw.h"


//*****************************************************************************
// Constants
//*****************************************************************************

// Yaw channels A and B use pins PB0 and PB1.
#define YAW_GPIO_PERIPH             SYSCTL_PERIPH_GPIOB
#define YAW_GPIO_BASE               GPIO_PORTB_BASE
#define YAW_CHANNEL_A_PIN           GPIO_PIN_0
#define YAW_CHANNEL_B_PIN           GPIO_PIN_1

// Yaw reference signal uses pin PC4.
#define YAW_REFERENCE_GPIO_PERIPH   SYSCTL_PERIPH_GPIOC
#define YAW_REFERENCE_GPIO_BASE     GPIO_PORTC_BASE
#define YAW_REFERENCE_PIN           GPIO_PIN_4

#define SLOTS_IN_CIRCLE             112
#define DEGREES_IN_CIRCLE           360
#define YAW_CHANGE_PER_SLOT         4

// The step size used when rotating the helicopter to find the reference point.
#define YAW_FIND_REFERENCE_STEP     10


//*****************************************************************************
// Static variables
//*****************************************************************************

// Yaw value relative to reference. Each slot corresponds to a yaw change of 4.
static int16_t yawChange = 0;
static int16_t desiredYaw = 0;


//*****************************************************************************
// Static function forward declarations.
//*****************************************************************************
static void yawChannelIntHandler(void);
static void yawReferenceIntHandler(void);


//*****************************************************************************
// Performs initialisation of the GPIO pins and interrupts used for
// measuring the yaw.
//*****************************************************************************
void initYaw(void) {
    // Configure the GPIO pins used for measuring the two yaw channels.
    SysCtlPeripheralEnable(YAW_GPIO_PERIPH);
    GPIOPinTypeGPIOInput(YAW_GPIO_BASE, YAW_CHANNEL_A_PIN | YAW_CHANNEL_B_PIN);

    // Configure a pin change interrupt to be triggered by both rising and
    // falling edges on channels A and B, and enable the interrupt.
    GPIOIntRegister(YAW_GPIO_BASE, yawChannelIntHandler);
    GPIOIntTypeSet(YAW_GPIO_BASE,
                   YAW_CHANNEL_A_PIN | YAW_CHANNEL_B_PIN,
                   GPIO_BOTH_EDGES);
    GPIOIntEnable(YAW_GPIO_BASE, YAW_CHANNEL_A_PIN | YAW_CHANNEL_B_PIN);

    // Configure the GPIO pin used to read the yaw reference signal.
    SysCtlPeripheralEnable(YAW_REFERENCE_GPIO_PERIPH);
    GPIOPinTypeGPIOInput(YAW_REFERENCE_GPIO_BASE, YAW_REFERENCE_PIN);
    GPIOPadConfigSet(YAW_REFERENCE_GPIO_BASE, YAW_REFERENCE_PIN,
                     GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Configure a pin change interrupt to be triggered by falling edges
    // in the yaw reference signal, since it is active low.
    GPIOIntRegister(YAW_REFERENCE_GPIO_BASE, yawReferenceIntHandler);
    GPIOIntTypeSet(YAW_REFERENCE_GPIO_BASE, YAW_REFERENCE_PIN, GPIO_FALLING_EDGE);
    GPIOIntEnable(YAW_REFERENCE_GPIO_BASE, YAW_REFERENCE_PIN);
}

//*****************************************************************************
// The pin change interrupt handler for the pins used to measure yaw
// channels A and B. Compares the current values of the two input yaw
// channels to their previous values and updates the yaw value as needed.
//*****************************************************************************
static void yawChannelIntHandler(void) {
    // Variables to keep track of whether each yaw channel was HIGH the last
    // time this interrupt occurred.
    static bool previousChannelA = false;
    static bool previousChannelB = false;

    // Check whether each yaw channel is currently HIGH.
    bool currentChannelA = GPIOPinRead(YAW_GPIO_BASE, YAW_CHANNEL_A_PIN)
                           == YAW_CHANNEL_A_PIN;
    bool currentChannelB = GPIOPinRead(YAW_GPIO_BASE, YAW_CHANNEL_B_PIN)
                           == YAW_CHANNEL_B_PIN;

    if (!previousChannelA &&  !previousChannelB) {
        if (!currentChannelA && currentChannelB) {
            yawChange += 1;
        } else if (currentChannelA && !currentChannelB) {
            yawChange -= 1;
        }
    } else if (!previousChannelA && previousChannelB) {
        if (currentChannelA && currentChannelB) {
            yawChange += 1;
        } else if (!currentChannelA && !currentChannelB) {
            yawChange -= 1;
        }
    } else if (previousChannelA && !previousChannelB) {
        if (!currentChannelA && !currentChannelB) {
            yawChange += 1;
        } else if (currentChannelA && currentChannelB) {
            yawChange -= 1;
        }
    } else {
        if (currentChannelA && !currentChannelB) {
            yawChange += 1;
        } else if (!currentChannelA && currentChannelB) {
            yawChange -= 1;
        }
    }

    previousChannelA = currentChannelA;
    previousChannelB = currentChannelB;

    GPIOIntClear(YAW_GPIO_BASE, YAW_CHANNEL_A_PIN | YAW_CHANNEL_B_PIN);
}

//*****************************************************************************
// The pin change interrupt handler for the yaw reference pin. If the
// helicopter is currently taking off, and therefore trying to find the yaw
// reference point, the interrupt handler will reset the current yaw value
// to zero. Otherwise the interrupt is ignored.
//*****************************************************************************
static void yawReferenceIntHandler(void) {
    if (flightState == TAKING_OFF) {
        yawChange = 0;
        desiredYaw = 0;
        flightState = FLYING;
    } else if (flightState == LANDING_FINDING_REFERENCE) {
        //TODO (mct63): Move or document this.
        yawChange = 0;
        desiredYaw = 0;
        flightState = LANDING;
        altitudeChangeDesired(-altitudePercent());
    }

    GPIOIntClear(YAW_REFERENCE_GPIO_BASE, YAW_REFERENCE_PIN);
}

//*****************************************************************************
// If the helicopter is currently taking off, the desired yaw is incremented
// periodically to rotate the helicopter in steps until the yaw reference point
// is found.
//*****************************************************************************
void yawFindReference (void) {
    // TODO (mct63): Document.
    if (flightState == TAKING_OFF || flightState == LANDING_FINDING_REFERENCE) {
        yawChangeDesired(YAW_FIND_REFERENCE_STEP);
    }
}

//*****************************************************************************
// Calculate and return the yaw in degrees, relative to the reference position.
//*****************************************************************************
int16_t yawDegrees(void) {
    // Convert yawChange to degrees.
    int16_t degrees = yawChange * DEGREES_IN_CIRCLE / SLOTS_IN_CIRCLE
                      / YAW_CHANGE_PER_SLOT;
    // Find remainder when divided by the number of degrees in a circle.
//    degrees = degrees % DEGREES_IN_CIRCLE;

    // Don't limit yaw range for now, to simplify control.

    // Ensure the returned value is between -180 and 179.
//    if (degrees < -DEGREES_IN_CIRCLE / 2) {
//        degrees += DEGREES_IN_CIRCLE;
//    } else if (degrees >= DEGREES_IN_CIRCLE / 2) {
//        degrees -= DEGREES_IN_CIRCLE;
//    }
    return degrees;
}

//*****************************************************************************
// Adds the given amount to the desired yaw.
//*****************************************************************************
void yawChangeDesired(int16_t amount) {
    desiredYaw += amount;
}

//*****************************************************************************
// Returns the desired yaw in degrees.
//*****************************************************************************
int16_t yawDesired(void) {
    return desiredYaw;
}

//*****************************************************************************
// Calculates and returns the difference between the desired yaw and the
// actual yaw, in degrees.
//*****************************************************************************
int16_t yawError(void) {
    return desiredYaw - yawDegrees();
}

