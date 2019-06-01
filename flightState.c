//*****************************************************************************
//
// File: flightState.c
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
//
// Defines the states which the helicopter can be in during its flight, and
// declares a global state variable to allow the state to be accessed and
// modified from multiple files.
//
//*****************************************************************************

#include "flightState.h"


// The current state of the helicopter.
static flightState_t flightState;


//*****************************************************************************
// Returns the current state of the helicopter.
//*****************************************************************************
flightState_t getFlightState(void) {
    return flightState;
}

//*****************************************************************************
// Sets the current state to the given value.
//*****************************************************************************
void setFlightState(flightState_t state) {
    flightState = state;
}

//*****************************************************************************
// Returns the current state as a string to be displayed.
//*****************************************************************************
char* flightStateString(void) {
    switch (flightState) {
        case LANDED: return "Landed";
        case FINDING_YAW_REFERENCE: return "Taking off";
        case FLYING: return "Flying";
        case LANDING_YAW: return "Landing";
        case LANDING_ALTITUDE: return "Landing";
        default: return "";
    }
}
