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
#include "flightState.h"

#include "control.h"


//*****************************************************************************
// Constants
//*****************************************************************************
#define CONTROL_KP_ALTITUDE         6
#define CONTROL_KD_ALTITUDE         1
#define CONTROL_KI_ALTITUDE         2
#define CONTROL_KP_YAW              10
#define CONTROL_KD_YAW              3
#define CONTROL_KI_YAW              2


//*****************************************************************************
// Static variables
//*****************************************************************************
static int16_t controlUpdateRate;

static int16_t altitudeErrorPrevious = 0;        // Units: %
static int32_t altitudeErrorIntegrated = 0;      // Units: 0.01%

static int16_t yawErrorPrevious = 0;             // Units: deg
static int32_t yawErrorIntegrated = 0;           // Units: 0.01 deg


//*****************************************************************************
// Static function forward declarations
//*****************************************************************************
static void controlUpdateAltitude(void);
static void controlUpdateYaw(void);


//*****************************************************************************
// Initialise the control module.
//*****************************************************************************
void initControl(int16_t updateRate) {
    controlUpdateRate = updateRate;
}

//*****************************************************************************
// Updates the main and tail motor duty cylces, based on the current altitude
// and yaw errors.
//*****************************************************************************
void controlUpdate(void) {
    controlUpdateAltitude();
    controlUpdateYaw();
}

//*****************************************************************************
// Update the main motor duty cycle based on the current altitude and the
// desired altitude.
//*****************************************************************************
static void controlUpdateAltitude(void) {
    int16_t error = altitudeError();
    int16_t errorDerivative = (error - altitudeErrorPrevious)
                               * controlUpdateRate;
    int32_t newIntegratedError = altitudeErrorIntegrated
                                  + error * 100 / controlUpdateRate;

    int16_t mainRotorDuty = (CONTROL_KP_ALTITUDE * error * 100
                             + CONTROL_KD_ALTITUDE * errorDerivative * 100
                             + CONTROL_KI_ALTITUDE * newIntegratedError) / 1000;

    altitudeErrorPrevious = error;

    if (mainRotorDuty > PWM_MAX_DUTY && error > 0) {
        mainRotorDuty = PWM_MAX_DUTY;
    } else if (mainRotorDuty < PWM_MAIN_MIN_DUTY && error < 0) {
        mainRotorDuty = PWM_MAIN_MIN_DUTY;
    } else {
        // Only accumulate error signal if output is within its limits,
        // to prevent integral windup.
        altitudeErrorIntegrated = newIntegratedError;
    }

    setMainRotorPower(mainRotorDuty);
}

//*****************************************************************************
// Update the tail motor duty cycle based on the current yaw and the
// desired yaw.
//*****************************************************************************
static void controlUpdateYaw(void) {
    int16_t error = yawError();
    int16_t errorDerivative = (error - yawErrorPrevious) * controlUpdateRate;
    int32_t newIntegratedError = yawErrorIntegrated
                                  + error * 100 / controlUpdateRate;

    int16_t tailRotorDuty = (CONTROL_KP_YAW * error * 100
                        + CONTROL_KD_YAW * errorDerivative * 100
                        + CONTROL_KI_YAW * newIntegratedError) / 1000;

    yawErrorPrevious = error;

    if (tailRotorDuty > PWM_MAX_DUTY && error > 0) {
        tailRotorDuty = PWM_MAX_DUTY;
    } else if (tailRotorDuty < PWM_TAIL_MIN_DUTY && error < 0) {
        tailRotorDuty = PWM_TAIL_MIN_DUTY;
    } else {
        // Only accumulate error signal if output is within its limits,
        // to prevent integral windup.
        yawErrorIntegrated = newIntegratedError;
    }

    setTailRotorPower(tailRotorDuty);
}





