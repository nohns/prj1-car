#pragma once

#define MOTOR_DIRECTION_FORWARD 1
#define MOTOR_DIRECTION_BACKWARD -1

class Motor
{
public:
    // Initialize motor driver
    Motor();

    // Accelerate motor up to speed. Speed given in m/s
    void accelerate(unsigned int toSpeed, unsigned char accelerationRate = 0);

    // Decelerate motor down from speed. Speed given in m/s
    void decelerate(unsigned int toSpeed, unsigned char accelerationRate = 0);

    unsigned int getSpeed();
    char getDirection();

    void setDirection(char direction);

private:
    unsigned char currentDutyCycle_;

    void writeDirection(char direction);
    void writeDutyCycle(const unsigned int dutyCycle);

    char readDirection();
    unsigned int readDutyCycle();
};