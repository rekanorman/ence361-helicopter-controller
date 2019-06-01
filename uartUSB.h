// ****************************************************************************
//
// File: uartUSB.h
//
// Authors: Reka Norman (rkn24)
//          Matthew Toohey (mct63)
//          James Brazier (jbr185)
//
// Attribution: Based on code from uartDemo.c,
//              written by P.J. Bones UCECE
//
// Support for transmission across a serial link using UART0
// on the Tiva board.
//
// Uses 9600 baud, 8-bit word length, 1 stop bit, no parity bit.
//
// ****************************************************************************

#ifndef UARTUSB_H_
#define UARTUSB_H_


//*****************************************************************************
// Initialise the UART module, including the Rx and Tx pins used.
//*****************************************************************************
void initUart(void);

//*****************************************************************************
// Transmits a message containing information about the status of the program.
//*****************************************************************************
void uartSendStatus(void);

//*****************************************************************************
// Transmit the given string via UART.
// Uses a blocking function for sending characters.
//*****************************************************************************
void uartSend(char *string);


#endif /* UARTUSB_H_ */
