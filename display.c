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
#include "altitude.h"
#include "yaw.h"

#include "display.h"


//*****************************************************************************
// Constants
//*****************************************************************************
// Maximum string length which can be displayed on the OLED display.
#define MAX_STR_LEN     16


//*****************************************************************************
// Initialise the display.
//*****************************************************************************
void initDisplay(void) {
    // Intialise the Orbit OLED display
    OLEDInitialise();
}

//*****************************************************************************
// Displays the appropriate information on the OLED display.
//*****************************************************************************
void displayUpdate(void) {
    char line[MAX_STR_LEN + 1];

    usnprintf(line, sizeof(line), "Alt: %5d%%     ", altitudePercent());
    OLEDStringDraw(line, 0, 0);

    usnprintf(line, sizeof(line), "Yaw: %5d deg  ", yawDegrees());
    OLEDStringDraw(line, 0, 1);

    usnprintf(line, sizeof(line), "Main: %4d%%     ", yawDegrees());
    OLEDStringDraw(line, 0, 2);

    usnprintf(line, sizeof(line), "Tail: %4d%%     ", yawDegrees());
    OLEDStringDraw(line, 0, 3);
}
