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
// Update the main motor duty cycle based on the current altitude and the
// desired altitude.
//*****************************************************************************
void controlUpdateAltitude(void);

//*****************************************************************************
// Update the tail motor duty cycle based on the current yaw and the
// desired yaw.
//*****************************************************************************
void controlUpdateYaw(void);


#endif  // CONTROL_H_
