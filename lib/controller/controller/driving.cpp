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
    lightDriverPtr_->turnOnTaillight();

    soundDriverPtr_->playSound(START_SOUND);

    // Start with a speed of 70
    drive(70);
}

void DrivingControl::handleReflex(unsigned char reflexNo)
{
    // On every reflex play sound
    soundDriverPtr_->playSound(REFLEX_SOUND);

    SendString("Reflex no: ");
    SendInteger(reflexNo);
    SendChar('\n');

    /*lightDriverPtr_->turnOnTaillight();
    _delay_ms(50);
    lightDriverPtr_->turnOffTaillight();*/

    // Alle steder der er kommentar til en command, er den originale kode.
    switch (reflexNo)
    {
    case 0:
        // If reflex is 0, then do nothing but wait a bit so reflex doesn't get triggered twice
        _delay_ms(200);
        break;
    case 1:
        // If reflex is 1, then drive with a speed of 80 to drive up the ramp
        _delay_ms(400);
        drive(30);
        break;
    case 2:
        // If reflex is 2, then slow down
        drive(80);
        break;
    case 3:
        // If reflex is 3 then drive with a speed of 80 after the ramp
        drive(0, 80);
        _delay_ms(20);
        reverse(60, 50);

        _delay_ms(100);
        drive(80);
    case 4:
        // If reflex is 3 then drive with a speed of 80 after the ramp
        _delay_ms(300);
        break;
    case 5:
        //_delay_ms(100);
        drive(60);
        break;
    case 6:
        // If reflex is 5, then brake and afterwards reverse with a speed of 70
        lightDriverPtr_->engageBrakeLight();
        drive(0, 60);
        _delay_ms(500);
        lightDriverPtr_->disengageBrakeLight();

        reverse(70);
        break;
    case 7:
        // If reflex is 6, then do nothing but wait a bit so reflex doesn't get triggered twice
        _delay_ms(300);
        break;
    case 8:
        // If reflex is 7, then drive with a speed of 70
        lightDriverPtr_->engageBrakeLight();
        reverse(0, 50);
        _delay_ms(500);
        lightDriverPtr_->disengageBrakeLight();

        drive(70);
        break;
    case 9:
        // If reflex is 8, then do nothing but wait a bit so reflex doesn't get triggered twice
        _delay_ms(300);
        break;
    case 10:
        // If reflex is 9, then do nothing but wait a bit so reflex doesn't get triggered twice
        _delay_ms(300);
        break;
    case 11:
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
    drive(0);
    _delay_ms(500);
    lightDriverPtr_->disengageBrakeLight();
    lightDriverPtr_->turnOffHeadlight();
    lightDriverPtr_->turnOffTaillight();

    // Play end sound
    _delay_ms(5000);
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
