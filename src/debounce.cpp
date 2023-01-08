// 8 jan 2023
#include "debounce.h"

void DebouncedButton::begin(uint8_t buttonPin)
{
    pin = buttonPin;
    state = 0;
    pinMode(pin, INPUT_PULLUP);
}

bool DebouncedButton::isPressed()
{
    state = (state << 1) | digitalRead(pin) | 0xfe00;
    return (state == 0xff00);
}