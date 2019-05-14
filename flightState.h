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
enum flightStates {LANDED = 0, TAKING_OFF, FLYING, LANDING};
typedef enum flightStates flightState_t;


// The current state of the helicopter.
extern flightState_t flightState;


#endif  // FLIGHT_STATE_H_
