// 8 jan 2023
#ifndef DEBOUNCE_H
#define DEBOUNCE_H

#include <Arduino.h>

class DebouncedButton
{
private:
    uint8_t pin;
    uint16_t state;

public:

    void begin(uint8_t buttonPin);
    bool isPressed();
};

#endif