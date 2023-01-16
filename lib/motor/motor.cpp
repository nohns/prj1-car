#include "motor.h"
#include <avr/io.h>
#include <math.h>
#include "uart.h"
#include <util/delay.h>

/* ----------------------------------- *
 *  Define internal private constants  *
 *  for motor driver                   *
 * ----------------------------------- */

#define MOTOR_ACCELERATION_PER_TICK 14 // Acceleration of 2 procent points every tick (one tick = 125ms)
#define MOTOR_DECELERATION_PER_TICK 32 // Deceleration of 6 procent points of duty cycle every tick (one tick = 125ms)

#define MOTOR_DIRECTION_DDR DDRH
#define MOTOR_DIRECTION_PORT PORTH
#define MOTOR_DIRECTION_PIN PH4

/* ----------------------------------- *
 *  Motor driver logic                 *
 * ----------------------------------- */

// Initialize motor driver. Requires PWM driver as dependency
Motor::Motor()
{
    // Set direction pin to output
    MOTOR_DIRECTION_DDR |= (1 << MOTOR_DIRECTION_PIN);

    // Initialize PWM
    // Mode = 3 (PWM, Phase Correct, 10 bit)
    // Set OC1A on match down counting / Clear OC1A on match up counting
    // Clock prescaler = 1
    TCCR4A = 0b10000011;
    TCCR4B = 0b00000001;
    writeDutyCycle(currentDutyCycle_);

    // Set PWM pin to output
    DDRH |= (1 << PH3);
    PORTH &= ~(1 << PH3);
}

void Motor::accelerate(unsigned int toSpeed, unsigned char accelerationRate)
{
    // Make sure toSpeed is above the current speed and speed is less than 100
    if (readDutyCycle() >= toSpeed || toSpeed > 100)
    {
        return;
    }

    unsigned int finalSpeed = readDutyCycle();
    SendString("Accelerating...\n");
    // initAccelerationTimer();
    while (finalSpeed < toSpeed)
    {
        // Wait for acceleration timer to fire
        // waitForAccelerationTimer();
        _delay_ms(75);

        // Increment speed
        finalSpeed += accelerationRate != 0 ? accelerationRate : MOTOR_ACCELERATION_PER_TICK;
        finalSpeed = finalSpeed > toSpeed ? toSpeed : finalSpeed;
        SendString("Writing new speed = ");
        SendInteger(finalSpeed);
        SendChar('\n');

        // Update pwm duty cycle
        writeDutyCycle(finalSpeed);
    }

    // stopAccelerationTimer();
}

void Motor::decelerate(unsigned int toSpeed, unsigned char accelerationRate)
{
    // Make sure toSpeed is below the current speed or speed is less than 0
    if (readDutyCycle() <= toSpeed || toSpeed < 0)
    {
        return;
    }

    SendString("Decelerating...\n");

    unsigned finalSpeed = readDutyCycle();
    while (finalSpeed > toSpeed)
    {

        // Decrement speed
        unsigned char accelerationTickrate = accelerationRate != 0 ? accelerationRate : MOTOR_DECELERATION_PER_TICK;
        finalSpeed = finalSpeed < accelerationTickrate ? 0 : finalSpeed - accelerationTickrate;

        // Update pwm duty cycle
        writeDutyCycle(finalSpeed);

        // Wait one tick
        _delay_ms(75);
    }
}

char Motor::getDirection()
{
    return readDirection();
}

unsigned int Motor::getSpeed()
{
    return readDutyCycle();
}

void Motor::setDirection(char direction)
{
    writeDirection(direction);
}

unsigned int Motor::readDutyCycle()
{
    // If OCR register is 0 then avoid dividing by 0
    if (OCR4A == 0)
    {
        return 0;
    }
    unsigned int dutyCycle = ((unsigned long)100 * OCR4A) / 1023;
    SendString("Read dutyCycle = ");
    SendInteger(dutyCycle);
    SendString(" and OCR4A = ");
    SendInteger(OCR4A);
    SendChar('\n');

    return dutyCycle;
}

void Motor::writeDirection(char direction)
{
    // Make sure direction is given
    if (direction != MOTOR_DIRECTION_FORWARD && direction != MOTOR_DIRECTION_BACKWARD)
    {
        return;
    }

    // Only change direction if dutycycle is 0
    if (readDutyCycle() == 0)
    {
        if (direction == MOTOR_DIRECTION_FORWARD)
        {
            // Set direction pin to low if direction is forward
            MOTOR_DIRECTION_PORT &= ~(1 << MOTOR_DIRECTION_PIN);
        }
        else
        {
            // Set direction pin to high if direction is backward
            MOTOR_DIRECTION_PORT |= (1 << MOTOR_DIRECTION_PIN);
        }
    }
}

void Motor::writeDutyCycle(const unsigned int dutyCycle)
{
    // If duty cycle given is 0 then avoid dividing by 0
    SendString("OCR4A = ");
    if (dutyCycle == 0)
    {
        OCR4A = 0;
        SendInteger(OCR4A);
        SendChar('\n');
        return;
    }

    unsigned long dutyCycleLong = (unsigned long)1023 * dutyCycle;
    OCR4A = dutyCycleLong / 100;
    SendInteger(OCR4A);
    SendChar('\n');
}

char Motor::readDirection()
{
    if (MOTOR_DIRECTION_PORT & (1 << MOTOR_DIRECTION_PIN))
    {
        return MOTOR_DIRECTION_BACKWARD;
    }

    return MOTOR_DIRECTION_FORWARD;
}