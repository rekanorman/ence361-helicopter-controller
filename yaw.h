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
// Performs initialisation of the GPIO pins and interrupts used for
// measuring the yaw.
//*****************************************************************************
void initYaw(void);

//*****************************************************************************
// If the helicopter is currently taking off, the desired yaw is incremented
// to rotate the helicopter in steps until the yaw reference point is found.
//*****************************************************************************
void yawFindReference (void);

//*****************************************************************************
// Calculate and return the yaw in degrees, relative to the reference position.
//*****************************************************************************
int16_t yawDegrees(void);

//*****************************************************************************
// Adds the given amount to the desired yaw.
//*****************************************************************************
void yawChangeDesired(int16_t amount);

//*****************************************************************************
// Returns the desired yaw in degrees.
//*****************************************************************************
int16_t yawDesired(void);

//*****************************************************************************
// Calculates and returns the difference between the desired yaw and the
// actual yaw, in degrees.
//*****************************************************************************
int16_t yawError(void);


#endif  // YAW_H_
