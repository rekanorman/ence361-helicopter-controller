//*****************************************************************************
//
// File: rotors.c
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
//
// Module for controlling the rotor blades of the helicopter.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"

#include "rotors.h"


//*****************************************************************************
// Constants
//*****************************************************************************

#define PWM_DIVIDER_CODE           SYSCTL_PWMDIV_4
#define PWM_DIVIDER                4

// Main rotor.
#define PWM_MAIN_ROTOR_FREQUENCY   250
#define PWM_MAIN_ROTOR_BASE        PWM0_BASE
#define PWM_MAIN_ROTOR_GEN         PWM_GEN_3
#define PWM_MAIN_ROTOR_OUTNUM      PWM_OUT_7
#define PWM_MAIN_ROTOR_OUTBIT      PWM_OUT_7_BIT
#define PWM_MAIN_ROTOR_PERIPH_PWM  SYSCTL_PERIPH_PWM0
#define PWM_MAIN_ROTOR_PERIPH_GPIO SYSCTL_PERIPH_GPIOC
#define PWM_MAIN_ROTOR_GPIO_BASE   GPIO_PORTC_BASE
#define PWM_MAIN_ROTOR_GPIO_CONFIG GPIO_PC5_M0PWM7
#define PWM_MAIN_ROTOR_GPIO_PIN    GPIO_PIN_5

// Tail rotor.
#define PWM_TAIL_ROTOR_FREQUENCY   250
#define PWM_TAIL_ROTOR_BASE        PWM1_BASE
#define PWM_TAIL_ROTOR_GEN         PWM_GEN_2
#define PWM_TAIL_ROTOR_OUTNUM      PWM_OUT_5
#define PWM_TAIL_ROTOR_OUTBIT      PWM_OUT_5_BIT
#define PWM_TAIL_ROTOR_PERIPH_PWM  SYSCTL_PERIPH_PWM1
#define PWM_TAIL_ROTOR_PERIPH_GPIO SYSCTL_PERIPH_GPIOF
#define PWM_TAIL_ROTOR_GPIO_BASE   GPIO_PORTF_BASE
#define PWM_TAIL_ROTOR_GPIO_CONFIG GPIO_PF1_M1PWM5
#define PWM_TAIL_ROTOR_GPIO_PIN    GPIO_PIN_1


//*****************************************************************************
// Static variables
//*****************************************************************************

// Percentage power of rotors.
static uint16_t mainRotorPower = 0;
static uint16_t tailRotorPower = 0;


//*****************************************************************************
// Static function forward declarations
//*****************************************************************************
static void initialiseMainRotor();
static void initialiseTailRotor();
static uint32_t calculatePulsePeriod(uint32_t frequency);
static uint32_t calculatePulseWidth(uint32_t dutyCycle, uint32_t period);


//*****************************************************************************
// Performs all initialisation needed for the rotors module.
//*****************************************************************************
void initRotors() {
    SysCtlPWMClockSet(PWM_DIVIDER_CODE);
    initialiseMainRotor();
    initialiseTailRotor();
}

//*****************************************************************************
// Performs initialisation for the main rotor.
//*****************************************************************************
static void initialiseMainRotor() {
    SysCtlPeripheralEnable(PWM_MAIN_ROTOR_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_MAIN_ROTOR_PERIPH_GPIO);

    GPIOPinConfigure(PWM_MAIN_ROTOR_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_MAIN_ROTOR_GPIO_BASE, PWM_MAIN_ROTOR_GPIO_PIN);

    PWMGenConfigure(PWM_MAIN_ROTOR_BASE,
                    PWM_MAIN_ROTOR_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenEnable(PWM_MAIN_ROTOR_BASE, PWM_MAIN_ROTOR_GEN);

    // Initially disable PWM output until rotor needs to start.
    PWMOutputState(PWM_MAIN_ROTOR_BASE, PWM_MAIN_ROTOR_OUTBIT, false);
}

//*****************************************************************************
// Performs initialisation for the tail rotor.
//*****************************************************************************
static void initialiseTailRotor() {
    SysCtlPeripheralEnable(PWM_TAIL_ROTOR_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_TAIL_ROTOR_PERIPH_GPIO);

    GPIOPinConfigure(PWM_TAIL_ROTOR_GPIO_CONFIG);
    GPIOPinTypePWM(PWM_TAIL_ROTOR_GPIO_BASE, PWM_TAIL_ROTOR_GPIO_PIN);

    PWMGenConfigure(PWM_TAIL_ROTOR_BASE, PWM_TAIL_ROTOR_GEN,
                        PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenEnable(PWM_TAIL_ROTOR_BASE, PWM_TAIL_ROTOR_GEN);

    // Initially disable PWM output until rotor needs to start.
    PWMOutputState(PWM_TAIL_ROTOR_BASE, PWM_TAIL_ROTOR_OUTBIT, false);
}

//*****************************************************************************
// Starts the main rotor, setting the duty cycle to the minimum value.
//*****************************************************************************
void startMainRotor() {
    PWMOutputState(PWM_MAIN_ROTOR_BASE, PWM_MAIN_ROTOR_OUTBIT, true);
    setMainRotorPower(PWM_MAIN_MIN_DUTY);
}

//*****************************************************************************
// Starts the tail rotor, setting the duty cycle to the minimum value.
//*****************************************************************************
void startTailRotor() {
    PWMOutputState(PWM_TAIL_ROTOR_BASE, PWM_TAIL_ROTOR_OUTBIT, true);
    setTailRotorPower(PWM_TAIL_MIN_DUTY);
}

//*****************************************************************************
// Calculates the period of the PWM signal (in clock ticks) for the given
// frequency.
//
// frequency: The frequency in Hz.
// returns:   The period in number of clock ticks.
//*****************************************************************************
static uint32_t calculatePulsePeriod(uint32_t frequency) {
    return SysCtlClockGet() / PWM_DIVIDER / frequency;
}

//*****************************************************************************
// Calculates the width of the PWM pulse (in PWM ticks) for the given
// duty cycle and period.
//
// dutyCycle: The duty cycle of the PWM signal.
// period:    The period of the PWM signal in number of clock ticks.
// returns:   The width of the PWM pulse in number of PWM clock ticks.
//*****************************************************************************
static uint32_t calculatePulseWidth(uint32_t dutyCycle, uint32_t period) {
    return period * dutyCycle / 100;
}

//*****************************************************************************
// Sets the power of the main rotor.
//
// power: The power level percentage to set the main rotor to.
//*****************************************************************************
void setMainRotorPower(int16_t power) {
    if (power > PWM_MAX_DUTY) {
        power = PWM_MAX_DUTY;
    } else if (power < PWM_MAIN_MIN_DUTY) {
        power = PWM_MAIN_MIN_DUTY;
    }
    uint32_t pulsePeriod = calculatePulsePeriod(PWM_MAIN_ROTOR_FREQUENCY);
    uint32_t pulseWidth = calculatePulseWidth(power, pulsePeriod);

    PWMGenPeriodSet(PWM_MAIN_ROTOR_BASE, PWM_MAIN_ROTOR_GEN, pulsePeriod);
    PWMPulseWidthSet(PWM_MAIN_ROTOR_BASE, PWM_MAIN_ROTOR_OUTNUM, pulseWidth);
    mainRotorPower = power;
}

//*****************************************************************************
// Sets the power of the tail rotor.
//
// power: The power level percentage to set the tail rotor to.
//*****************************************************************************
void setTailRotorPower(int16_t power) {
    if (power > PWM_MAX_DUTY) {
        power = PWM_MAX_DUTY;
    } else if (power < PWM_TAIL_MIN_DUTY) {
        power = PWM_TAIL_MIN_DUTY;
    }
    uint32_t pulsePeriod = calculatePulsePeriod(PWM_TAIL_ROTOR_FREQUENCY);
    uint32_t pulseWidth = calculatePulseWidth(power, pulsePeriod);

    PWMGenPeriodSet(PWM_TAIL_ROTOR_BASE, PWM_TAIL_ROTOR_GEN, pulsePeriod);
    PWMPulseWidthSet(PWM_TAIL_ROTOR_BASE, PWM_TAIL_ROTOR_OUTNUM, pulseWidth);
    tailRotorPower = power;
}

//*****************************************************************************
// Gets the power of the main rotor.
//
// returns: The power level percentage of the main rotor.
//*****************************************************************************
uint16_t getMainRotorPower() {
    return mainRotorPower;
}

//*****************************************************************************
// Gets the power of the tail rotor.
//
// returns: The power level percentage of the tail rotor.
//*****************************************************************************
uint16_t getTailRotorPower() {
    return tailRotorPower;
}
