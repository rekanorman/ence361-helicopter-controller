//*****************************************************************************
//
// File: altitude.c
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
//
// Module for measuring the altitude by taking regular ADC samples and
// averaging them.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/sysctl.h"
#include "circBufT.h"
#include "altitude.h"


//*****************************************************************************
// Constants
//*****************************************************************************
#define BUF_SIZE                40

// The range over which the ADC sample values vary, from landed to fully up.
// Calculated as: 4095 * (0.8V / 3.3V)
#define ADC_RANGE               993

//*****************************************************************************
// Constants related to ADC.
//*****************************************************************************
#define ALTITUDE_ADC_PERIPH             SYSCTL_PERIPH_ADC0
#define ALTITUDE_ADC_BASE               ADC0_BASE
#define ALTITUDE_ADC_SEQUENCE           3
#define ALTITUDE_ADC_SEQUENCE_PRIORITY  0    // Only one sequence used so priority doesn't matter.
#define ALTITUDE_ADC_STEP               0

// ADC input channel to be used. Should be set to ADC_CTL_CH0 for testing
// with the potentiometer, and ADC_CTL_CH9 for actual altitude input.
#define ALTITUDE_ADC_CHANNEL            ADC_CTL_CH9


//*****************************************************************************
// Static variables
//*****************************************************************************

// Circular buffer of size BUFFER_SIZE containing the ADC samples.
static circBuf_t inBuffer;

// Number of ADC samples taken, used to check whether buffer is filled yet.
static uint32_t numSamplesTaken = 0;

static int32_t meanADC;       // Current mean ADC value.
static int32_t sumADC;        // Current sum of the ADC samples in the buffer.
static int32_t referenceADC;  // ADC value corresponding to 'landed' altitude.


//*****************************************************************************
// Static function forward declarations.
//*****************************************************************************
static void initAltitudeADC(void);
static void altitudeADCIntHandler(void);


//*****************************************************************************
// Performs all initialisation needed for the altitude module.
//*****************************************************************************
void initAltitude(void) {
    initCircBuf(&inBuffer, BUF_SIZE);
    initAltitudeADC();
}

//*****************************************************************************
// Initialises the ADC module used to sample the altitude.
//*****************************************************************************
static void initAltitudeADC(void) {
    // The ADC peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(ALTITUDE_ADC_PERIPH);

    // Enable the sample sequence with a processor signal trigger.
    ADCSequenceConfigure(ALTITUDE_ADC_BASE,
                         ALTITUDE_ADC_SEQUENCE,
                         ADC_TRIGGER_PROCESSOR,
                         ALTITUDE_ADC_SEQUENCE_PRIORITY);

    // Configure the ADC step. The ADC channel is sampled in single-ended mode
    // (default), the interrupt flag is to be set when the sample is done,
    // and this is the last conversion done on the sequence.
    ADCSequenceStepConfigure(ALTITUDE_ADC_BASE,
                             ALTITUDE_ADC_SEQUENCE,
                             ALTITUDE_ADC_STEP,
                             ALTITUDE_ADC_CHANNEL | ADC_CTL_IE | ADC_CTL_END);

    // Enable the sequence.
    ADCSequenceEnable(ALTITUDE_ADC_BASE, ALTITUDE_ADC_SEQUENCE);

    // Register the conversion complete interrupt handler.
    ADCIntRegister(ALTITUDE_ADC_BASE,
                   ALTITUDE_ADC_SEQUENCE,
                   altitudeADCIntHandler);

    // Enable interrupts for the ADC sequence (clears any outstanding interrupts)
    ADCIntEnable(ALTITUDE_ADC_BASE, ALTITUDE_ADC_SEQUENCE);
}

//*****************************************************************************
// Waits until enough samples have been taken to fill the buffer, then
// initialises the reference ADC value to the current mean ADC value.
// Should be called after all ADC-related initialisation has been done, to
// ensure that ADC interrupts have started.
//*****************************************************************************
void altitudeSetInitialReference(void) {
    // Wait for the buffer to be filled before setting the reference value.
    while (numSamplesTaken < BUF_SIZE) {}
    referenceADC = meanADC;
}

//*****************************************************************************
// Set the reference ADC value to the current mean ADC value.
//*****************************************************************************
void altitudeResetReference(void) {
    referenceADC = meanADC;
}

//*****************************************************************************
// Triggers a conversion on the ADC sequence being used to measure the
// altitude. Should be called at a rate equal to the desired sampling rate.
//*****************************************************************************
void altitudeTriggerConversion(void) {
    ADCProcessorTrigger(ALTITUDE_ADC_BASE, ALTITUDE_ADC_SEQUENCE);
}

//*****************************************************************************
// The handler for the ADC conversion complete interrupt.
// Reads the oldest value in the circular buffer before writing the the new
// value, and uses these two values to recalculate the mean ADC value.
//*****************************************************************************
static void altitudeADCIntHandler(void) {
    uint32_t oldestValue, newValue;

    // Get the new sample from the ADC module.
    ADCSequenceDataGet(ALTITUDE_ADC_BASE, ALTITUDE_ADC_SEQUENCE, &newValue);
    numSamplesTaken++;

    // Get the oldest value from the circular buffer (before it is overwritten).
    oldestValue = readEarliestValueCircBuf(&inBuffer);

    // Write the new value to the circular buffer.
    writeCircBuf(&inBuffer, newValue);

    // Adjust the current sum and calculate new mean ADC.
    sumADC = sumADC - oldestValue + newValue;
    meanADC = (2 * sumADC + BUF_SIZE) / 2 / BUF_SIZE;

    // Clean up, clearing the interrupt
    ADCIntClear(ALTITUDE_ADC_BASE, ALTITUDE_ADC_SEQUENCE);
}

//*****************************************************************************
// Returns the mean of the ADC samples currently in the buffer.
//*****************************************************************************
uint32_t altitudeMeanADC(void) {
    return meanADC;
}

//*****************************************************************************
// Calculates and returns the current percentage altitude, based on the mean
// sample value and relative to the global referenceSample, which represents
// the landed altitude. Percentage can be positive or negative
// TODO(everyone): If this is needed for the control, it should be calculated every time
//       a sample is taken, but for now it's just being used for display.
//*****************************************************************************
int16_t altitudePercent(void) {
    return (referenceADC - meanADC) * 100 / ADC_RANGE;
}