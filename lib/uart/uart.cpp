#include "uart.h"

#include <avr/io.h>
#include <stdlib.h>
#include "uart.h"

// Target CPU frequency
#define XTAL 16000000

// UART 0 initialization:

// Changed from 2 to 0
UART::UART()
{
    // Enable RX and TX
    UCSR0B = 0b00011000;
    // Asynchronous mode, 1 stop bit, no parity, 8 data bits
    UCSR0C = 0b00000110;
    // Set baud rate to 9600
    UBRR0 = XTAL / 16 / 9600 - 1;
}

/*************************************************************************
Awaits transmitter register ready.
Then send the character.
Parameter :
    Tegn : Character for sending.
*************************************************************************/
void UART::sendChar(char Tegn)
{
    // Changed from 2 to 0
    //  Wait for transmitter register empty (ready for new character)
    while ((UCSR0A & (1 << 5)) == 0)
    {
    }
    // Then send the character
    UDR0 = Tegn;
}

/*************************************************************************
Sends 0 terminated string.
Parameter:
   Streng: Pointer to the string.
*************************************************************************/
void UART::sendCommand(char *array)
{
    // Loops through array
    for (int i = 0; i < 8; i++)
    {
        char tmp = array[i];
        sendChar(tmp);
    }
}