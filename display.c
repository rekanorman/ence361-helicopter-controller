//*****************************************************************************
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
                    DISPLAY_YAW,
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
    char string[MAX_STR_LEN + 1];

    if (displayState == DISPLAY_ALTITUDE_PERCENT) {
        usnprintf(string, sizeof(string), "Altitude: %5d%%", altitudePercent);

    } else if (displayState == DISPLAY_MEAN_ADC) {
        usnprintf(string, sizeof(string), "Mean ADC: %4d  ", altitudeMeanADC);

    } else if (displayState == DISPLAY_YAW) {
        usnprintf(string, sizeof(string), "Yaw: %4d deg   ", yawDegrees);

    } else {
        usnprintf(string, sizeof(string), "                ");
    }

    OLEDStringDraw(string, 0, 0);
}

//*****************************************************************************
// Cycle to the next display state.
//*****************************************************************************
void displayStateUpdate(void) {
    displayState = (displayState + 1) % NUM_DISPLAY_STATES;
}
