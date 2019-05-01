//*****************************************************************************
//
// File: yaw.h
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
//
// Module measuring the yaw angle using pin change interrupts.
//
//*****************************************************************************

#ifndef YAW_H_
#define YAW_H_


//*****************************************************************************
// Performs initialisation for measuring the yaw.
//*****************************************************************************
void initYaw(void);

//*****************************************************************************
// Reset the yaw value to zero.
//*****************************************************************************
void yawReset(void);

//*****************************************************************************
// Calculate and return the yaw in dregrees, relative to the position when
// the program started.
//*****************************************************************************
int16_t yawDegrees(void);


#endif  // YAW_H_
