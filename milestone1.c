//*****************************************************************************
//
// ADCdemo1.c - Simple interrupt driven program which samples with AIN9.
//
// This version sends the ADC status across a serial link, using funcions
// in the uartUSB module, as well as displaying it on the OLED display.
//
// Author:  P.J. Bones  UCECE
// Last modified:   8.2.2018
//
//*****************************************************************************
// Based on the 'convert' series from 2016
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "buttons4.h"
#include "circBufT.h"
#include "OrbitOLED/OrbitOLEDInterface.h"

//*****************************************************************************
// Constants
//*****************************************************************************
enum displayStates {DISPLAY_ALTITUDE_PERCENT = 0,
                    DISPLAY_MEAN_ADC,
                    DISPLAY_BLANK,
                    NUM_DISPLAY_STATES
                   };

#define BUF_SIZE                40
#define SAMPLE_RATE_HZ          400
#define DISPLAY_UPDATE_RATE_HZ  5
#define MAX_STR_LEN             16

// ADC_CTL_CH0 for testing with potentiometer, ADC_CTL_CH9 for altitude input.
#define ADC_INPUT_CHANNEL       ADC_CTL_CH9

// The range over which the ADC sample values vary, from landed to fully up.
// Calculated as: 4095 * (0.8V / 3V)
#define SAMPLE_RANGE            1092

//*****************************************************************************
// Global variables
//*****************************************************************************
static circBuf_t inBuffer;                  // Buffer of size BUF_SIZE integers (sample values)
static uint8_t displayUpdateTick = false;  // When true, should update display
static uint8_t displayState = DISPLAY_ALTITUDE_PERCENT;
static uint16_t referenceSample;  // Mean ADC sample corresponding to 'landed' altitude

//*****************************************************************************
// The interrupt handler for the for SysTick interrupt.
//*****************************************************************************
void SysTickIntHandler(void) {
    // Initiate a conversion.
    ADCProcessorTrigger(ADC0_BASE, 3);

    // Update the buttons.
    updateButtons();

    // Check if it is time to update the display.
    const uint16_t ticksPerUpdate = SAMPLE_RATE_HZ / DISPLAY_UPDATE_RATE_HZ;
    static uint16_t tickCount = 0;
    if (++tickCount >= ticksPerUpdate) {
        tickCount = 0;
        displayUpdateTick = true;
    }
}

//*****************************************************************************
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//*****************************************************************************
void ADCIntHandler(void) {
    uint32_t ulValue;

    // Get the single sample from ADC0.
    ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);

    // Place it in the circular buffer (advancing write index)
    writeCircBuf(&inBuffer, ulValue);

    // Clean up, clearing the interrupt
    ADCIntClear(ADC0_BASE, 3);
}

//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick)
//*****************************************************************************
void initClock(void) {
    // Set the clock rate to 20 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                    SYSCTL_XTAL_16MHZ);

    // Set up the period for the SysTick timer.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);

    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);

    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}

void initADC(void) {
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_INPUT_CHANNEL | ADC_CTL_IE |
                             ADC_CTL_END);

    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);

    // Register the interrupt handler
    ADCIntRegister(ADC0_BASE, 3, ADCIntHandler);

    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}

void initDisplay(void) {
    // intialise the Orbit OLED display
    OLEDInitialise();
}

//*****************************************************************************
// Calculates and returns the rounded mean of the samples currently in the
// buffer.
// Note: should not be called until at least BUFF_SIZE samples have been taken.
//*****************************************************************************
uint16_t calculateMeanSample(void) {
    uint32_t sum = 0, i = 0;
    for (i = 0; i < BUF_SIZE; i++) {
        sum = sum + readCircBuf(&inBuffer);
    }
    return (2 * sum + BUF_SIZE) / 2 / BUF_SIZE;
}

//*****************************************************************************
// Calculates and returns the current percentage altitude, based on the mean
// sample value and relative to the global referenceSample, which represents
// the landed altitude.
//*****************************************************************************
uint16_t calculateAltitudePercent(uint16_t meanSample) {
    // Voltage decreases as altitude increases, so if the mean sample is
    // greater than the reference, the helicopter must be landed.
    if (meanSample > referenceSample) {
        return 0;
    }

    return (referenceSample - meanSample) * 100 / SAMPLE_RANGE;
}

//*****************************************************************************
// Checks if the UP or LEFT buttons have been pushed, and if so carries out
// the appropriate actions.
// Note: buttons are updated regularly in the SysTickIntHandler.
//*****************************************************************************
void checkButtons(void) {
    // Cycle the display when the UP button is pushed.
    if (checkButton(UP) == PUSHED) {
        displayState = (displayState + 1) % NUM_DISPLAY_STATES;
    }

    // Recalculate the reference sample value when the LEFT button is pushed.
    if (checkButton(LEFT) == PUSHED) {
        referenceSample = calculateMeanSample();
    }
}

//*****************************************************************************
// Calculates the current mean ADC sample and percentage altitude, and
// displays the appropriate information to the OLED display, based on the
// value of the global displayState.
//*****************************************************************************
void updateDisplay(void) {
    uint16_t meanSample = calculateMeanSample();
    uint16_t altitudePercent = calculateAltitudePercent(meanSample);

    char string[MAX_STR_LEN + 1];

    if (displayState == DISPLAY_ALTITUDE_PERCENT) {
        usnprintf(string, sizeof(string), "Altitude: %3d%%", altitudePercent);

    } else if (displayState == DISPLAY_MEAN_ADC) {
        usnprintf(string, sizeof(string), "Mean ADC: %4d", meanSample);

    } else {
        usnprintf(string, sizeof(string), "                ");
    }

    OLEDStringDraw(string, 0, 0);
}

int main(void) {
    initClock();
    initADC();

    // Disable SysTick and ADC interrupts during initialisation.
    IntMasterDisable();

    initDisplay();
    initCircBuf(&inBuffer, BUF_SIZE);
    initButtons();

    // Enable interrupts to the processor once initialisation is complete.
    IntMasterEnable();

    // Wait 0.5s so that the buffer to be filled with samples, then calculate
    // the initial reference sample value.
    SysCtlDelay(SysCtlClockGet() / 6);
    referenceSample = calculateMeanSample();

    while (1) {
        checkButtons();
        if (displayUpdateTick) {
            displayUpdateTick = false;
            updateDisplay();
        }
    }
}
