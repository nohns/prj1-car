#include "sound.h"
#include "uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

Sound::Sound(UART *uartDriver)
{
    uart = uartDriver;

    // Volume set to max
    char volume[8] = {0x7E, 0x06, 0x00, 0x00, 0x1E, 0xFF, 0xDC, 0xEF};
    uart->sendCommand(volume);

    // Setting to play from sdCard
    char source[8] = {0x7E, 0x09, 0x00, 0x00, 0x02, 0xFF, 0xF5, 0xEF};
    uart->sendCommand(source);

    // Setting equlizer to bass
    char equ[] = {0x7E, 0x07, 0x00, 0x00, 0x05, 0xFF, 0xF4, 0xEF};
    uart->sendCommand(equ);
}

void Sound::playSound(int song)
{
    switch (song)
    {
    case 1:
    {
        _delay_ms(500); // Make delay to avoid somo not being ready

        // Play start track
        char startTrack[] = {0x7E, 0x0F, 0x00, 0x01, 0x01, 0xFF, 0xEF, 0xEF};
        uart->sendCommand(startTrack);
        break;
    }
    case 2:
    {
        // Plays reflex sound
        char reflexSound[] = {0x7E, 0x0F, 0x00, 0x01, 0x02, 0xFF, 0xEE, 0xEF};
        uart->sendCommand(reflexSound);
        break;
    }
    case 3:
    {
        // Plays stop sound
        char stopSound[] = {0x7E, 0x0F, 0x00, 0x01, 0x03, 0xFF, 0xED, 0xEF};
        uart->sendCommand(stopSound);
        break;
    }
    default:
        break;
    };
}