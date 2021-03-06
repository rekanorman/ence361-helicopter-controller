//*****************************************************************************
//
// File: flightState.h
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

#ifndef FLIGHT_STATE_H_
#define FLIGHT_STATE_H_


// A type representing the possible states of the helicopter.
enum flightStates {LANDED = 0,             // Stationary with rotors off
                   FINDING_YAW_REFERENCE,  // Rotating in steps to find reference
                   FLYING,
                   LANDING_YAW,       // Restoring the yaw to zero for landing
                   LANDING_ALTITUDE}; // Restoring the altitude to zero for landing

typedef enum flightStates flightState_t;


//*****************************************************************************
// Returns the current state of the helicopter.
//*****************************************************************************
flightState_t getFlightState(void);

//*****************************************************************************
// Sets the current state to the given value.
//*****************************************************************************
void setFlightState(flightState_t state);

//*****************************************************************************
// Returns the current state as a string to be displayed.
//*****************************************************************************
char* flightStateString(void);


#endif  // FLIGHT_STATE_H_
