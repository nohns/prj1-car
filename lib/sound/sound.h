#pragma once
#include "uart.h"

#define START_SOUND 1
#define REFLEX_SOUND 2
#define END_SOUND 3

class Sound
{
public:
    Sound(UART *uartDriver);

    void playSound(int song);

private:
    UART *uart;
};