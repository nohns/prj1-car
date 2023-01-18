#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "uart.h"
#include "sound.h"
#include "motor.h"
#include "light.h"
#include "controller/driving.h"

/* ---------------------------------- *
 *  Define constants for main program *
 * ---------------------------------- */

// Define what port and pin number the start button is connected to
#define START_BTN_PORT PORTA
#define START_BTN_PIN PINA
#define START_BTN_DDR DDRA
#define START_BTN_BIT PA0

/* ---------------------------------- */

/* ---------------------------------- *
 *  Main program of car               *
 * ---------------------------------- */

void waitForStartPress();
void initSensors();
void onReflexEncountered();
void disableExternalInterrupts();
void enableExternalInterrupts();

// Variable for storing pointer to driving control
DrivingControl *drivingControlPtr;

// Variable for storing amount of reflexes encountered
volatile unsigned int reflexesEncountered = 0;

int main(void)
{
    InitUART(9600, 8);

    // Wait until start signal is given
    SendString("wait for start...\n");
    waitForStartPress();

    // Initialize dependency tree (drivers)
    UART uartDriver;
    Motor motorDriver;
    Sound soundDriver(&uartDriver);
    Light lightDriver;
    DrivingControl divingControl(&motorDriver, &soundDriver, &lightDriver);
    drivingControlPtr = &divingControl;

    // Enable interrupts globally
    sei();

    SendString("Starting to drive...\n");
    drivingControlPtr->start();

    // Init sensors on side of car
    initSensors();

    // Wait for reflexes to be encountered
    while (1)
    {
    }
}

// Waits for a complete press of the start button (down and up)
void waitForStartPress()
{
    char pinMask = 1 << START_BTN_BIT;

    // Configure start button pin as input and with pull-up resistor
    START_BTN_DDR &= pinMask;  // CLEAR
    START_BTN_PORT |= pinMask; // SET

    // Wait for start button down
    while ((START_BTN_PIN & pinMask) == 0)
    {
        SendString(START_BTN_PIN & pinMask ? "1\n" : "0\n");
    }

    SendString("DOWN now wating for up");
    // Wait for start button up, to complete press
    while (START_BTN_PIN & pinMask)
    {
    }
}

// Enables external interrupts INT2 and INT3 for sensors on the side of the car
void initSensors()
{
    EICRA |= 0b11110000; // Set INT2 and INT3 to trigger on falling edge
    EIMSK |= 0b00001100; // Enable exernal interrupt INT2 (PD2) and INT3 (PD3)
}

// Interrupt for reflex 1
ISR(INT2_vect)
{
    onReflexEncountered();
}

// Interrupt for reflex 2
ISR(INT3_vect)
{
    onReflexEncountered();
}

void onReflexEncountered()
{
    disableExternalInterrupts();

    // Handling driving instructions
    drivingControlPtr->handleReflex(reflexesEncountered);
    reflexesEncountered++;

    _delay_ms(100);
    enableExternalInterrupts();
}

// Disables external interrupts INT2 and INT3 for sensors on the side of the car
void disableExternalInterrupts()
{
    EIMSK &= ~((1 << INT2) | (1 << INT3)); // Disable external interrupt INT2 (PD2) and INT3 (PD3)
    EIFR |= ((1 << INTF2) | (1 << INTF3)); // Clear interrupt flag INTF2 and INTF3
}

// Enables external interrupts INT2 and INT3 for sensors on the side of the car
void enableExternalInterrupts()
{
    EIMSK |= ((1 << INT2) | (1 << INT3)); // Enable external interrupt INT2 (PD2)
}

/* ---------------------------------- */