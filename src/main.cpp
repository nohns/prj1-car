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

/************************************************************************/

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

    // Init sensors
    initSensors();

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

#define MAX_INSTRUCTIONS_PER_REFLEX 5
#define MAX_REFLEXES 14

/*const char instructions[MAX_REFLEXES][MAX_INSTRUCTIONS_PER_REFLEX] = {
    {
        0,
    }, // Reflex 1 - dont do anything
    {1, 80},
    {1, 0},
    {1, 80},
    {0},
    {1, -70},
    {0},
    {1, 70},
    {0},
    {0},
    {1, 0},
    {1, 0},
    {1, 0}};*/

void disableExternalInterrupts()
{
    EIMSK &= ~((1 << INT2) | (1 << INT3)); // Disable external interrupt INT2 (PD2) and INT3 (PD3)
    EIFR |= ((1 << INTF2) | (1 << INTF3)); // Clear interrupt flag INTF2 and INTF3
}

void enableExternalInterrupts()
{
    EIMSK |= ((1 << INT2) | (1 << INT3)); // Enable external interrupt INT2 (PD2)
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

ISR(INT2_vect)
{
    onReflexEncountered();

    /*const char *reflexInstructions = instructions[reflexesEncountered];
    SendString("Running reflex instruction ");
    SendInteger(reflexesEncountered);
    SendString("...\n");

    DDRB |= (1 << PB7);
    PORTB |= (1 << PB7);

    char numberOfInstructions = reflexInstructions[0];
    for (char i = 1; i <= numberOfInstructions; i++)
    {
        char instruction = reflexInstructions[i];

        SendString("Instruction value is ");
        SendInteger(instruction);
        SendChar('\n');

        if (instruction == 0)
        {
            drivingControlPtr->brake();
        }
        else if (instruction > 0)
        {
            drivingControlPtr->drive(instruction);
        }
        else
        {
            drivingControlPtr->reverse(-instruction);
        }
    }

    // Make delay so we dont get multiple fires
    if (numberOfInstructions == 0)
    {
        _delay_ms(300);
    }
    else
    {
        _delay_ms(125);
    }*/
}

ISR(INT3_vect)
{
    onReflexEncountered();
}

/* ---------------------------------- */