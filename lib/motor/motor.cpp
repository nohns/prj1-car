#include "motor.h"
#include <avr/io.h>
#include <math.h>
#include "uart.h"
#include <util/delay.h>

/* ----------------------------------- *
 *  Define internal private constants  *
 *  for motor driver                   *
 * ----------------------------------- */

#define MOTOR_DEFAULT_ACCELERATION_TICK_MS 75
#define MOTOR_ACCELERATION_PER_TICK 14 // Acceleration in procent points every tick
#define MOTOR_DECELERATION_PER_TICK 32 // Deceleration in procent points of duty cycle every tick

#define MOTOR_DIRECTION_DDR DDRH
#define MOTOR_DIRECTION_PORT PORTH
#define MOTOR_DIRECTION_PIN PH4

/* ----------------------------------- *
 *  Motor driver logic                 *
 * ----------------------------------- */

// Initialize motor driver
Motor::Motor()
{
    // Set direction pin to output
    MOTOR_DIRECTION_DDR |= (1 << MOTOR_DIRECTION_PIN);

    // Initialize PWM
    // Mode = 3 (PWM, Phase Correct, 10 bit), Prescaler = 64, (~120Hz)
    // Set OC1A on match down counting / Clear OC1A on match up counting
    TCCR4A = 0b10000011;
    TCCR4B = 0b00000011;
    writeDutyCycle(currentDutyCycle_);

    // Set PWM pin to output
    DDRH |= (1 << PH3);
    PORTH &= ~(1 << PH3);
}

void Motor::accelerate(unsigned char toSpeed, unsigned char accelerationRate)
{
    // Make sure toSpeed is above the current speed and speed is less than 100
    if (readDutyCycle() >= toSpeed || toSpeed > 100)
    {
        return;
    }

    unsigned char finalSpeed = readDutyCycle();
    SendString("Accelerating...\n");
    // initAccelerationTimer();
    while (finalSpeed < toSpeed)
    {
        // Increment speed
        finalSpeed += accelerationRate != 0 ? accelerationRate : MOTOR_ACCELERATION_PER_TICK;
        // finalSpeed += (accelerationRate != 0 ? accelerationRate : MOTOR_ACCELERATION_PER_TICK) / 2;
        finalSpeed = finalSpeed > toSpeed ? toSpeed : finalSpeed;
        SendString("Writing new speed = ");
        SendInteger(finalSpeed);
        SendChar('\n');

        // Update pwm duty cycle
        writeDutyCycle(finalSpeed);

        _delay_ms(MOTOR_DEFAULT_ACCELERATION_TICK_MS);
    }

    // stopAccelerationTimer();
}

void Motor::decelerate(unsigned char toSpeed, unsigned char accelerationRate)
{
    // Make sure toSpeed is below the current speed or speed is less than 0
    if (readDutyCycle() <= toSpeed || toSpeed < 0)
    {
        return;
    }

    SendString("Decelerating...\n");

    unsigned char finalSpeed = readDutyCycle();
    while (finalSpeed > toSpeed)
    {

        // Decrement speed
        unsigned char accelerationTickrate = accelerationRate != 0 ? accelerationRate : MOTOR_DECELERATION_PER_TICK;
        // unsigned char accelerationTickrate = (accelerationRate != 0 ? accelerationRate : MOTOR_DECELERATION_PER_TICK) / 2;
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

unsigned char Motor::getSpeed()
{
    return readDutyCycle();
}

void Motor::setDirection(char direction)
{
    writeDirection(direction);
}

unsigned char Motor::readDutyCycle()
{
    // If OCR register is 0 then avoid dividing by 0
    if (OCR4A == 0)
    {
        return 0;
    }
    unsigned int dutyCycle = ((unsigned long)100 * OCR4A) / 1023;
    // unsigned int dutyCycle = ((unsigned long)100 * OCR4A) / 511;
    SendString("Read dutyCycle = ");
    SendInteger(dutyCycle);
    SendString(" and OCR4A = ");
    SendInteger(OCR4A);
    SendChar('\n');

    return dutyCycle;
}

void Motor::writeDirection(char direction)
{
    // Only change direction if motor is stopped
    if (readDutyCycle() > 0)
    {
        return;
    }

    // Make sure valid direction is given
    if (direction != MOTOR_DIRECTION_FORWARD && direction != MOTOR_DIRECTION_BACKWARD)
    {
        return;
    }

    // Change direction
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

void Motor::writeDutyCycle(const unsigned char dutyCycle)
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
    // unsigned long dutyCycleLong = (unsigned long)511 * dutyCycle;
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