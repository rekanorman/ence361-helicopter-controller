//*****************************************************************************
//
// File: control.c
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
//
// Module implementing PI control for the altitude and yaw of the helicopter.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "altitude.h"
#include "yaw.h"
#include "rotors.h"

#include "control.h"


//*****************************************************************************
// Constants
//*****************************************************************************
#define CONTROL_KP_ALTITUDE         1
#define CONTROL_KI_ALTITUDE         0.2
#define CONTROL_KP_YAW              1
#define CONTROL_KI_YAW              0.5


//*****************************************************************************
// Static variables
//*****************************************************************************
static int16_t controlUpdateRate;
//static double altitudeErrorIntegrated = 0;      // Units: 0.01%
static double yawErrorIntegrated = 0;           // Units: 0.01 deg


//*****************************************************************************
// Initialise the control module.
//*****************************************************************************
void initControl(int16_t updateRate) {
    controlUpdateRate = updateRate;
}

//*****************************************************************************
// Update the main motor duty cycle based on the current altitude and the
// desired altitude.
//*****************************************************************************
void controlUpdateAltitude(void) {
//    int16_t error = altitudeError();
//    altitudeErrorIntegrated += error * 100 / CONTROL_UPDATE_FREQUENCY;
//
//    int16_t control = (CONTROL_KP_ALTITUDE * error * 100
//                       + CONTROL_KI_ALTITUDE * altitudeErrorIntegrated) / 100;


}

//*****************************************************************************
// Update the tail motor duty cycle based on the current yaw and the
// desired yaw.
//*****************************************************************************
void controlUpdateYaw(void) {
    int16_t error = yawError();
    yawErrorIntegrated += error * 100 / controlUpdateRate;

    int16_t tailRotorDuty = (CONTROL_KP_YAW * error * 100
                        + CONTROL_KI_YAW * yawErrorIntegrated) / 100;

    setTailRotorPower(tailRotorDuty);
}





