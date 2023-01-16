#pragma once

#include "motor.h"
#include "sound.h"
#include "light.h"

class DrivingControl
{
public:
    DrivingControl(Motor *motorDriver, Sound *soundDriver, Light *lightDriver);

    void start();

    // Handles a reflex and executes the corresponding instructions
    void handleReflex(unsigned char reflexNo);

    // Drives the car forward
    void drive(unsigned int speed, unsigned char accelerationRate = 0);

    // Brakes the car to a halt
    void brake();

    // Reverses the car backwards
    void reverse(unsigned int speed, unsigned char accelerationRate = 0);

private:
    Motor *motorDriverPtr_;
    Sound *soundDriverPtr_;
    Light *lightDriverPtr_;

    void end();
};