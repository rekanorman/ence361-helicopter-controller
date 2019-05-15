//*****************************************************************************
//
// File: switch.h
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
//
// Module for Switch 1 of the on the ORBIT daughter board.
//
//*****************************************************************************

#ifndef SWITCH_H_
#define SWITCH_H_

// Type for the state of the switch.
enum switchStates {SWITCH_UP = 0, SWITCH_DOWN};
typedef enum switchStates switch_state_t;

//*****************************************************************************
// Performs initialisation for the main switch.
//*****************************************************************************
void initSwitch();

//*****************************************************************************
// Checks the current state of state of SW1.
//
// returns: The current state of SW1, either SWITCH_UP or SWITCH_DOWN.
//*****************************************************************************
switch_state_t checkSwitch1();

#endif  // SWITCH_H_
