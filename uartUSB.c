// ****************************************************************************
//
// File: uartUSB.c
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


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "utils/ustdlib.h"
#include "altitude.h"
#include "yaw.h"
#include "rotors.h"
#include "flightState.h"

#include "uartUSB.h"


//****************************************************************
// Constants
//****************************************************************
#define BAUD_RATE           9600

// Uses UART0 module with Rx pin PA0 and Tx pin PA1.
#define UART_BASE           UART0_BASE
#define UART_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_GPIO_BASE      GPIO_PORTA_BASE
#define UART_GPIO_PIN_RX    GPIO_PIN_0
#define UART_GPIO_PIN_TX    GPIO_PIN_1
#define UART_GPIO_PINS      UART_GPIO_PIN_RX | UART_GPIO_PIN_TX

// Transmits with word length 8, one stop bit and no parity bits.
#define UART_WORD_LEN       UART_CONFIG_WLEN_8
#define UART_STOP_BIT       UART_CONFIG_STOP_ONE
#define UART_PAR_BIT        UART_CONFIG_PAR_NONE
#define UART_CONFIG         UART_WORD_LEN | UART_STOP_BIT | UART_PAR_BIT

// The number of characters to send over UART at a time.
#define STR_LEN             18


//*****************************************************************************
// Initialise the UART module, including the Rx and Tx pins used.
//*****************************************************************************
void initUart (void) {
    // Enable the UART0 module and GPIO port A, which is used for UART0 pins.
    SysCtlPeripheralEnable(UART_PERIPH_UART);
    SysCtlPeripheralEnable(UART_PERIPH_GPIO);

    // Configure the Rx and Tx pins for UART use.
    GPIOPinTypeUART(UART_GPIO_BASE, UART_GPIO_PINS);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    // Configure the UART clock rate, baud rate, word length, stop bits
    // and parity bits
    UARTConfigSetExpClk(UART_BASE, SysCtlClockGet(), BAUD_RATE, UART_CONFIG);

    // Enable Tx and Rx buffers and the UART module itself.
    UARTFIFOEnable(UART_BASE);
    UARTEnable(UART_BASE);
}

//*****************************************************************************
// Transmits a message containing information about the status of the program.
//*****************************************************************************
void uartSendStatus(void) {
    char line[STR_LEN + 1];

    usnprintf(line, sizeof(line),
              "Alt: %4d [%4d]\r\n", altitudePercent(), altitudeDesired());
    uartSend(line);

    usnprintf(line, sizeof(line),
              "Yaw: %4d [%4d]\r\n", yawDegrees(), yawDesired());
    uartSend(line);

    usnprintf(line, sizeof(line), "Main: %4d%%\r\n", getMainRotorPower());
    uartSend(line);

    usnprintf(line, sizeof(line), "Tail: %4d%%\r\n", getTailRotorPower());
    uartSend(line);

    usnprintf(line, sizeof(line), "%16s\r\n", flightStateString());
    uartSend(line);
}

//*****************************************************************************
// Transmit the given string via UART.
// Uses a blocking function for sending characters.
//*****************************************************************************
void uartSend(char *string) {
    while(*string) {
        // Write the next character to the UART Tx buffer.
        UARTCharPut(UART_BASE, *string);
        string++;
    }
}
