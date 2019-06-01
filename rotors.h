//*****************************************************************************
//
// File: rotors.h
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
//
// Module for controlling the motors powering the main and tail rotors of
// the helicopter.
//
//*****************************************************************************

#ifndef ROTORS_H_
#define ROTORS_H_


//*****************************************************************************
// Public constants
//*****************************************************************************

// Range of allowed duty cycles for the main and tail motors. The minimum main
// motor power is set to 20% to ensure that anticlockwise rotation is possible
// even when the main motor is at its minimum.
#define PWM_MAX_DUTY        95
#define PWM_MAIN_MIN_DUTY   20
#define PWM_TAIL_MIN_DUTY   5


//*****************************************************************************
// Performs all initialisation needed for the rotors module.
//*****************************************************************************
void initRotors();

//*****************************************************************************
// Starts the main rotor, setting the duty cycle to the minimum value.
//*****************************************************************************
void startMainRotor();

//*****************************************************************************
// Starts the tail rotor, setting the duty cycle to the minimum value.
//*****************************************************************************
void startTailRotor();

//*****************************************************************************
// Stops the main rotor, by disabling the PWM output.
//*****************************************************************************
void stopMainRotor();

//*****************************************************************************
// Stops the tail rotor, by disabling the PWM output.
//*****************************************************************************
void stopTailRotor();

//*****************************************************************************
// Sets the power of the main rotor.
//
// power: The power level percentage to set the main rotor to.
//*****************************************************************************
void setMainRotorPower(int16_t power);

//*****************************************************************************
// Sets the power of the tail rotor.
//
// power: The power level percentage to set the tail rotor to.
//*****************************************************************************
void setTailRotorPower(int16_t power);

//*****************************************************************************
// Gets the power of the main rotor.
//
// returns: The power level percentage of the main rotor.
//*****************************************************************************
uint16_t getMainRotorPower();

//*****************************************************************************
// Gets the power of the tail rotor.
//
// returns: The power level percentage of the tail rotor.
//*****************************************************************************
uint16_t getTailRotorPower();

#endif  // ROTORS_H_
