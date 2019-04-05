//*****************************************************************************
//
// Module for measuring the altitude by taking regular ADC samples and
// averaging them.
//
//*****************************************************************************

#ifndef ALTITUDE_H_
#define ALTITUDE_H_


//*****************************************************************************
// Performs all initialisation needed for the altitude module.
//*****************************************************************************
void initAltitude(void);

//*****************************************************************************
// Waits until enough samples have been taken to fill the buffer, then
// initialises the reference ADC value to the current mean ADC value.
// Should be called after all ADC-related initialisation has been done, to
// ensure that ADC interrupts have started.
//*****************************************************************************
void altitudeSetInitialReference(void);

//*****************************************************************************
// Set the reference ADC value to the current mean ADC value.
//*****************************************************************************
void altitudeResetReference(void);

//*****************************************************************************
// Triggers a conversion on the ADC sequence being used to measure the
// altitude. Should be called at a rate equal to the desired sampling rate.
//*****************************************************************************
void altitudeTriggerConversion(void);

//*****************************************************************************
// Returns the mean of the ADC samples currently in the buffer.
//*****************************************************************************
uint32_t altitudeMeanADC(void);

//*****************************************************************************
// Calculates and returns the current percentage altitude, based on the mean
// sample value and relative to the global referenceSample, which represents
// the landed altitude. Percentage can be positive or negative
//*****************************************************************************
int16_t altitudePercent(void);


#endif  // ALTITUDE_H_
