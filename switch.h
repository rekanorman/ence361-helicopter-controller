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


// Type for the state of the switch SWITCH_UP and SWITCH_DOWN mean that the
// switch has been moved since it was last checked.
enum switchStates {SWITCH_UNCHANGED = 0, SWITCH_UP, SWITCH_DOWN};
typedef enum switchStates switchState_t;


//*****************************************************************************
// Performs initialisation for the main switch.
//*****************************************************************************
void initSwitch();

//*****************************************************************************
// Checks if the switch position has changed since the last call, and updates
// the switch state as necessary.
//*****************************************************************************
void updateSwitch1(void);

//*****************************************************************************
// Returns the current state of the switch, indicating whether it has been
// switched up or down, or is unchanged since the last call to this function.
//*****************************************************************************
switchState_t checkSwitch1();


#endif  // SWITCH_H_
