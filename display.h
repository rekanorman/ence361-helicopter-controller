//*****************************************************************************
//
// File: display.h
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
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
// Displays the appropriate information on the OLED display.
//*****************************************************************************
void displayUpdate(void);


#endif  // DISPLAY_H_
