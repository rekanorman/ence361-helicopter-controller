//*****************************************************************************
//
// File: control.h
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
//
// Module implementing PI control for the altitude and yaw of the helicopter.
//
//*****************************************************************************

#ifndef CONTROL_H_
#define CONTROL_H_


//*****************************************************************************
// Initialise the control module.
//*****************************************************************************
void initControl(int16_t updateRate);

//*****************************************************************************
// Updates the main and tail motor duty cylces, based on the current altitude
// and yaw errors.
//*****************************************************************************
void controlUpdate(void);


#endif  // CONTROL_H_
