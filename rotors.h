//*****************************************************************************
//
// File: rotors.h
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
//
// Module for controlling the rotor blades of the helicopter.
//
//*****************************************************************************

#ifndef ROTORS_H_
#define ROTORS_H_

//*****************************************************************************
// Performs all initialisation needed for the rotors module.
//*****************************************************************************
void initRotors();

//*****************************************************************************
// Performs initialisation for the main rotor.
//*****************************************************************************
void initialiseMainRotor();

//*****************************************************************************
// Performs initialisation for the tail rotor.
//*****************************************************************************
void initialiseTailRotor();

//*****************************************************************************
// Enables the main rotor.
//*****************************************************************************
void enableMainRotor();

//*****************************************************************************
// Enables the tail rotor.
//*****************************************************************************
void enableTailRotor();

//*****************************************************************************
// Sets the power of the main rotor.
//
// power: The power level percentage to set the main rotor to.
//*****************************************************************************
void setMainRotorPower(uint16_t power);

//*****************************************************************************
// Sets the power of the tail rotor.
//
// power: The power level percentage to set the tail rotor to.
//*****************************************************************************
void setTailRotorPower(uint16_t power);

#endif  // ROTORS_H_
