// ****************************************************************************
//
// File: buttons4.h
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
//
// Attribution: Based on code by P.J. Bones UCECE
//
// Support for a set of FOUR specific buttons on the Tiva/Orbit.
// The buttons are:  UP and DOWN (on the Orbit daughterboard) plus
// LEFT and RIGHT on the Tiva.
//
// ****************************************************************************

#ifndef BUTTONS4_H_
#define BUTTONS4_H_


#include <stdint.h>
#include <stdbool.h>


//*****************************************************************************
// Constants
//*****************************************************************************
enum butNames {UP = 0, DOWN, LEFT, RIGHT, NUM_BUTS};
enum butStates {RELEASED = 0, PUSHED, NO_CHANGE};

typedef enum butNames buttonName_t;
typedef enum butStates buttonState_t;

// UP button
#define UP_BUT_PERIPH  SYSCTL_PERIPH_GPIOE
#define UP_BUT_PORT_BASE  GPIO_PORTE_BASE
#define UP_BUT_PIN  GPIO_PIN_0
#define UP_BUT_NORMAL  false
// DOWN button
#define DOWN_BUT_PERIPH  SYSCTL_PERIPH_GPIOD
#define DOWN_BUT_PORT_BASE  GPIO_PORTD_BASE
#define DOWN_BUT_PIN  GPIO_PIN_2
#define DOWN_BUT_NORMAL  false
// LEFT button
#define LEFT_BUT_PERIPH  SYSCTL_PERIPH_GPIOF
#define LEFT_BUT_PORT_BASE  GPIO_PORTF_BASE
#define LEFT_BUT_PIN  GPIO_PIN_4
#define LEFT_BUT_NORMAL  true
// RIGHT button
#define RIGHT_BUT_PERIPH  SYSCTL_PERIPH_GPIOF
#define RIGHT_BUT_PORT_BASE  GPIO_PORTF_BASE
#define RIGHT_BUT_PIN  GPIO_PIN_0
#define RIGHT_BUT_NORMAL  true

// Debounce algorithm: A state machine is associated with each button.
// A state change occurs only after NUM_BUT_POLLS consecutive polls have
// read the pin in the opposite condition, before the state changes and
// a flag is set.  Set NUM_BUT_POLLS according to the polling rate.
#define NUM_BUT_POLLS 3


// ****************************************************************************
// Initialise the variables associated with the set of buttons defined by the
// constants above.
// ****************************************************************************
void initButtons(void);

// ****************************************************************************
// Function designed to be called regularly. It polls all of the
// buttons once and updates variables associated with the buttons if
// necessary.  It is efficient enough to be part of an ISR, e.g. from
// a SysTick interrupt.
// ****************************************************************************
void updateButtons(void);

// ****************************************************************************
// Function returns the new button state if the button state
// (PUSHED or RELEASED) has changed since the last call, otherwise returns
// NO_CHANGE.  The argument butName should be one of constants in the
// enumeration butStates, excluding 'NUM_BUTS'. Safe under interrupt.
// ****************************************************************************
buttonState_t checkButton(buttonName_t butName);


#endif  // BUTTONS4_H_
