//*****************************************************************************
//
// File: altitude.h
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
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
// Triggers a conversion on the ADC sequence being used to measure the
// altitude. Should be called at a rate equal to the desired sampling rate.
//*****************************************************************************
void altitudeTriggerConversion(void);

//*****************************************************************************
// Returns the mean of the ADC samples currently in the buffer.
//*****************************************************************************
int16_t altitudeMeanADC(void);

//*****************************************************************************
// Calculates and returns the current percentage altitude, based on the mean
// sample value and relative to the global referenceSample, which represents
// the landed altitude. Percentage can be positive or negative
//*****************************************************************************
int16_t altitudePercent(void);

//*****************************************************************************
// Returns the desired percentage altitude.
//*****************************************************************************
int16_t altitudeDesired(void);

//*****************************************************************************
// Calculates and returns the difference between the desired altitude and
// the current altitude, as a percent.
//*****************************************************************************
int16_t altitudeError(void);


#endif  // ALTITUDE_H_
