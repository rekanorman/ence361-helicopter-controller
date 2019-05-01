//*****************************************************************************
//
// File: display.c
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
//
// Module for displaying information about the altitude and yaw angle on the
// OLED display.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "utils/ustdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "display.h"


//*****************************************************************************
// Constants
//*****************************************************************************
enum displayStates {DISPLAY_ALTITUDE_PERCENT = 0,
                    DISPLAY_MEAN_ADC,
                    DISPLAY_BLANK,
                    NUM_DISPLAY_STATES
                   };

// Maximum string length which can be displayed on the OLED display.
#define MAX_STR_LEN     16


//*****************************************************************************
// Static variables
//*****************************************************************************
static uint8_t displayState = DISPLAY_ALTITUDE_PERCENT;


//*****************************************************************************
// Initialise the display.
//*****************************************************************************
void initDisplay(void) {
    // Intialise the Orbit OLED display
    OLEDInitialise();
}

//*****************************************************************************
// Displays the appropriate information on the OLED display, based on the
// current displayState.
//*****************************************************************************
void displayUpdate(int16_t altitudePercent,
                   uint32_t altitudeMeanADC,
                   int16_t yawDegrees) {
    char line1[MAX_STR_LEN + 1];
    char line2[MAX_STR_LEN + 1];

    if (displayState == DISPLAY_ALTITUDE_PERCENT) {
        usnprintf(line1, sizeof(line1), "Altitude: %5d%%", altitudePercent);
        usnprintf(line2, sizeof(line2), "Yaw: %4d deg   ", yawDegrees);

    } else if (displayState == DISPLAY_MEAN_ADC) {
        usnprintf(line1, sizeof(line1), "Mean ADC: %4d  ", altitudeMeanADC);
        usnprintf(line2, sizeof(line2), "                ");

    } else {
        usnprintf(line1, sizeof(line1), "                ");
        usnprintf(line2, sizeof(line2), "                ");
    }

    OLEDStringDraw(line1, 0, 0);
    OLEDStringDraw(line2, 0, 2);
}

//*****************************************************************************
// Cycle to the next display state.
//*****************************************************************************
void displayStateUpdate(void) {
    displayState = (displayState + 1) % NUM_DISPLAY_STATES;
}
