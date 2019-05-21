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
// The yaw will be in the range -180 to 180 degrees.
//*****************************************************************************
int16_t yawDegrees(void);

//*****************************************************************************
// Adds the given amount to the desired yaw, ensuring that the desired yaw
// remains in the range of 180 to -180 degrees.
//*****************************************************************************
void yawChangeDesired(int16_t amount);

//*****************************************************************************
// Called when the helicopter is returning to the reference yaw for landing.
// Moves the desired yaw towards zero by the given amount. If the yaw has
// reached the reference, changes the state of the helicopter.
//*****************************************************************************
void yawUpdateLanding(int16_t amount);

//*****************************************************************************
// Returns the desired yaw in degrees.
//*****************************************************************************
int16_t yawDesired(void);

//*****************************************************************************
// Calculates and returns the difference between the desired yaw and the
// actual yaw in degrees, taking into account that both these values are in
// the range -180 to 180 degrees. Therefore the returned error will also be
// in this range.
//*****************************************************************************
int16_t yawError(void);


#endif  // YAW_H_
