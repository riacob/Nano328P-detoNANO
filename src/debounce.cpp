/**
 * @file debounce.cpp
 * @author Riccardo Iacob
 * @brief 
 * @version 0.1
 * @date 2023-01-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */
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