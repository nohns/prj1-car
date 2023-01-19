#include "driving.h"
#include "uart.h"
#include <avr/io.h>
#include "uart.h"
#include <util/delay.h>

DrivingControl::DrivingControl(Motor *motorDriver, Sound *soundDriver, Light *lightDriver)
    : motorDriverPtr_(motorDriver),
      soundDriverPtr_(soundDriver),
      lightDriverPtr_(lightDriver)
{
}

void DrivingControl::start()
{
    lightDriverPtr_->turnOnHeadlight();
    // lightDriverPtr_->turnOnTaillight();

    soundDriverPtr_->playSound(START_SOUND);
    _delay_ms(2000);

    // Start with a speed of 70
    drive(60);
}

void DrivingControl::handleReflex(unsigned char reflexNo)
{
    // On every reflex lower than the rest play sound
    if (reflexNo < 11)
    {
        soundDriverPtr_->playSound(REFLEX_SOUND);
    }

    SendString("Reflex no: ");
    SendInteger(reflexNo);
    SendChar('\n');

    lightDriverPtr_->turnOnTaillight();
    _delay_ms(60);
    lightDriverPtr_->turnOffTaillight();

    // Alle steder der er kommentar til en command, er den originale kode.
    switch (reflexNo)
    {
    case 0:
        // If reflex is 0, then do nothing but wait a bit so reflex doesn't get triggered twice
        _delay_ms(200);
        drive(40, 5);
        break;
    case 1:
        // If reflex is 2, then slow down
        drive(20, 10);
        _delay_ms(175);
        drive(70, 15);

        break;
    case 2:
        // If reflex is 3 then drive with a speed of 80 after the ramp
        drive(0, 50);
        //_delay_ms(50);
        reverse(20, 20);
        _delay_ms(1100);
        drive(70);
    case 3:
        // If reflex is 3 then drive with a speed of 80 after the ramp
        drive(60);
        _delay_ms(200);
        break;
    case 4:
        drive(60);
        _delay_ms(100);
        break;
    case 5:
        // If reflex is 5, then brake and afterwards reverse with a speed of 70
        // lightDriverPtr_->engageBrakeLight();
        drive(0, 60);
        _delay_ms(300);
        // lightDriverPtr_->disengageBrakeLight();

        reverse(60);
        break;
    case 6:
        // If reflex is 6, then do nothing but wait a bit so reflex doesn't get triggered twice
        _delay_ms(300);
        break;
    case 7:
        // If reflex is 7, then drive with a speed of 70
        // lightDriverPtr_->engageBrakeLight();
        reverse(0, 60);
        _delay_ms(300);
        // lightDriverPtr_->disengageBrakeLight();

        drive(70);

        break;
    case 8:
        // If reflex is 8, then do nothing but wait a bit so reflex doesn't get triggered twice
        _delay_ms(300);
        break;
    case 9:
        // If reflex is 9, then do nothing but wait a bit so reflex doesn't get triggered twice
        _delay_ms(200);
        break;
    case 10:
        // If reflex is 10, then stop the car
        end();
        _delay_ms(1000);
        break;
    default:
        drive(0);
        break;
    }

    /*lightDriverPtr_->turnOnHeadlight();
    _delay_ms(50);
    lightDriverPtr_->turnOffHeadlight();*/
}

void DrivingControl::end()
{
    // Brake the car to a halt with braking lights
    lightDriverPtr_->engageBrakeLight();
    drive(0, 80);
    _delay_ms(100);
    reverse(20, 0);
    _delay_ms(100);
    reverse(0);
    _delay_ms(500);
    lightDriverPtr_->disengageBrakeLight();
    lightDriverPtr_->turnOffHeadlight();
    lightDriverPtr_->turnOffTaillight();

    // Play end sound
    _delay_ms(1000);
    soundDriverPtr_->playSound(END_SOUND);
}

void DrivingControl::drive(unsigned int speed, unsigned char accelerationRate)
{
    // If direction is not forward, then decelerate the car to standstil and set direction to forward
    // Before getting the car to the new speed
    if (motorDriverPtr_->getDirection() != MOTOR_DIRECTION_FORWARD)
    {
        reverse(0, accelerationRate);
        motorDriverPtr_->setDirection(MOTOR_DIRECTION_FORWARD);
    }

    // Accelerate/Decelerate the motor to speed
    SendString("Driving to speed = ");
    SendInteger(speed);
    SendString(", motor speed = ");
    SendInteger(motorDriverPtr_->getSpeed());
    SendChar('\n');
    if (speed < motorDriverPtr_->getSpeed())
    {
        // If new speed is below how fast the car is currently driving, then decelerate
        motorDriverPtr_->decelerate(speed, accelerationRate);
    }
    else
    {
        // If new speed is above how fast the car is currently driving, then accelerate
        motorDriverPtr_->accelerate(speed, accelerationRate);
    }
}

void DrivingControl::brake()
{
    // Start brake lights when starting to brake
    lightDriverPtr_->engageBrakeLight();

    // Decelerate the motor to a standstill
    motorDriverPtr_->decelerate(0);

    // Stop brake lights when car finished braking
    lightDriverPtr_->disengageBrakeLight();
}

void DrivingControl::reverse(unsigned int speed, unsigned char accelerationRate)
{
    // If direction is not backward, then decelerate the car to standstil and set direction to backward
    // Before getting the car to the new speed
    if (motorDriverPtr_->getDirection() != MOTOR_DIRECTION_BACKWARD)
    {
        drive(0, accelerationRate);
        motorDriverPtr_->setDirection(MOTOR_DIRECTION_BACKWARD);
    }

    // Accelerate/Decelerate the motor to speed
    if (speed < motorDriverPtr_->getSpeed())
    {
        // If new speed is below how fast the car is currently driving, then decelerate
        motorDriverPtr_->decelerate(speed, accelerationRate);
    }
    else
    {
        // If new speed is above how fast the car is currently driving, then accelerate
        motorDriverPtr_->accelerate(speed, accelerationRate);
    }
}
