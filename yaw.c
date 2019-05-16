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
#define YAW_FIND_REFERENCE_STEP     15


//*****************************************************************************
// Static variables
//*****************************************************************************

// Yaw value relative to reference. Each slot corresponds to a yaw change of 4.
static int16_t yawChange = 0;

// The desired yaw value in degrees, in the range -180 to 180 degrees.
static int16_t desiredYaw = 0;


//*****************************************************************************
// Static function forward declarations.
//*****************************************************************************
static void yawChannelIntHandler(void);
static void yawReferenceIntHandler(void);
static int16_t convertYawToRange(int16_t yaw);


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
    if (getFlightState() == FINDING_YAW_REFERENCE) {
        yawChange = 0;
        desiredYaw = 0;
        setFlightState(FLYING);
    }

    GPIOIntClear(YAW_REFERENCE_GPIO_BASE, YAW_REFERENCE_PIN);
}

//*****************************************************************************
// If the helicopter is currently taking off, the desired yaw is incremented
// periodically to rotate the helicopter in steps until the yaw reference point
// is found.
//*****************************************************************************
void yawFindReference (void) {
    if (getFlightState() == FINDING_YAW_REFERENCE) {
        yawChangeDesired(YAW_FIND_REFERENCE_STEP);
    }
}

//*****************************************************************************
// Takes an arbitrary yaw value in degrees, and converts it to an equivalent
// value in the range of -180 to 180 degrees.
//*****************************************************************************
static int16_t convertYawToRange(int16_t yaw) {
    yaw %= DEGREES_IN_CIRCLE;
    if (yaw < -DEGREES_IN_CIRCLE / 2) {
        yaw += DEGREES_IN_CIRCLE;
    } else if (yaw >= DEGREES_IN_CIRCLE / 2) {
        yaw -= DEGREES_IN_CIRCLE;
    }
    return yaw;
}

//*****************************************************************************
// Calculate and return the yaw in degrees, relative to the reference position.
// The yaw will be in the range of -180 to 180 degrees.
//*****************************************************************************
int16_t yawDegrees(void) {
    // Convert yawChange to degrees.
    int16_t degrees = yawChange * DEGREES_IN_CIRCLE / SLOTS_IN_CIRCLE
                      / YAW_CHANGE_PER_SLOT;

    return convertYawToRange(degrees);
}

//*****************************************************************************
// Adds the given amount to the desired yaw, ensuring that the desired yaw
// remains in the range of 180 to -180 degrees.
//*****************************************************************************
void yawChangeDesired(int16_t amount) {
    desiredYaw = convertYawToRange(desiredYaw + amount);

}

//*****************************************************************************
// Sets the desired yaw to the given value, ensuring it is in the range of
// -180 to 180 degrees.
//*****************************************************************************
void yawSetDesired(int16_t yaw) {
    desiredYaw = convertYawToRange(yaw);
}

//*****************************************************************************
// Returns the desired yaw in degrees.
//*****************************************************************************
int16_t yawDesired(void) {
    return desiredYaw;
}

//*****************************************************************************
// Calculates and returns the difference between the desired yaw and the
// actual yaw in degrees, taking into account that both these values are in
// the range -180 to 180 degrees. Therefore the returned error will also be
// in this range.
//*****************************************************************************
int16_t yawError(void) {
    return convertYawToRange(desiredYaw - yawDegrees());
}
