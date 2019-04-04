//*****************************************************************************
//
// Module for displaying information about the altitude and yaw angle on the
// OLED display.
//
//*****************************************************************************

#ifndef DISPLAY_H_
#define DISPLAY_H_


//*****************************************************************************
// Initialise the display.
//*****************************************************************************
void initDisplay(void);

//*****************************************************************************
// Displays the appropriate information on the OLED display, based on the
// current displayState.
//*****************************************************************************
void displayUpdate(uint16_t altitudePercent,
                   uint32_t altitudeMeanADC,
                   uint16_t yawChange);

//*****************************************************************************
// Cycle to the next display state.
//*****************************************************************************
void displayStateUpdate(void);


#endif /* DISPLAY_H_ */
