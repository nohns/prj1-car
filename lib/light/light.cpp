#include <avr/io.h>
#include "light.h"

using namespace std;

const unsigned int headlightOCR1B = 65534;  // lysstyrke for forlys
const unsigned int taillightOCR1A = 14500;  // lysstyrke for baglys
const unsigned int brakelightOCR1A = 65534; // lysstyrke for bremselys
const unsigned int TOP = 65535;

Light::Light()
{
}

void Light::turnOnHeadlight()
{
    OCR1B = headlightOCR1B;

    // Set the Timer 1 to Fast PWM mode, with ICR1 as TOP, non-inverting mode
    TCCR1A |= (1 << COM1B1);
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM12) | (1 << WGM13);

    // Ingen prescaling
    TCCR1B |= (1 << CS10);

    // Set OC1B as the output compare pin, OC1A = PB6, set PB6 så forlys LED'erne lyser
    DDRB |= (1 << PB6);
    PORTB |= (1 << PB6);

    ICR1 = TOP; // Set TOP = 65535
}
void Light::turnOffHeadlight()
{
    // Disabler bit 6 igen for at slukke forlyset
    OCR1B = 0;
}
void Light::turnOnTaillight()
{
    OCR1A = taillightOCR1A;

    // Set the Timer 1 to Fast PWM mode, with ICR1 as TOP, non-inverting mode
    TCCR1A |= (1 << COM1A1);
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM12) | (1 << WGM13);

    // Ingen prescaling
    TCCR1B |= (1 << CS10);

    // Set OC1A as the output compare pin, OC1A = PB5, set PB5 så baglys LED'erne lyser
    DDRB |= (1 << PB5);
    PORTB |= (1 << PB5);

    ICR1 = TOP; // Set TOP = 65535
}
void Light::turnOffTaillight()
{
    // disabler bit 5 for at slukke baglyset igen
    OCR1A = 0;
}
void Light::engageBrakeLight()
{
    // Skriver højere duty cycle til baglys agerende som bremselys
    OCR1A = brakelightOCR1A;
}
void Light::disengageBrakeLight()
{
    // Reverter til alm. baglys
    OCR1A = taillightOCR1A;
}