/**
 * @file main_tx.cpp
 * @author Riccardo Iacob
 * @brief Main file for the transmitter
 * @version 0.1
 * @date 2023-01-08
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <Arduino.h>
#include <printf.h>
#include <SPI.h>
#include <Wire.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SSD1306/SSD1306Ascii.h>
#include <SSD1306/SSD1306AsciiAvrI2c.h>

#include "hardware.h"
#include "userconfig.h"
#include "debounce.h"
#include "screenstates.h"

// RF24
RF24 radio(PIN_RF24_CE, PIN_RF24_CSN);
// SSD1306
SSD1306AsciiAvrI2c oled;
// Debounced buttons
DebouncedButton btnCenter;
DebouncedButton btnLeft;
DebouncedButton btnRight;
// Configurations
userconfig_s masterConfig;
userconfig_s slaveConfig;

// The number of times the center button has been pressed
int screenIdx = 0;
// Wether or not the device is unlocked
uint8_t isUnlocked = false;

void abortISR();

void setup()
{
    // Initialize GPIOs
    pinMode(PIN_LED_R, OUTPUT);
    pinMode(PIN_LED_G, OUTPUT);
    pinMode(PIN_LED_B, OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(PIN_BTN_ABORT, INPUT_PULLUP);
    btnCenter.begin(PIN_BTN_CENTER);
    btnLeft.begin(PIN_BTN_LEFT);
    btnRight.begin(PIN_BTN_RIGHT);
    pinMode(PIN_RF24_IRQ, INPUT);
    pinMode(PIN_RF24_CE, OUTPUT);
    pinMode(PIN_RF24_CSN, OUTPUT);
    pinMode(PIN_ARM, INPUT);
    pinMode(PIN_DETONATE, INPUT);
    digitalWrite(PIN_BUZZER, LOW);

    // Initialize interrupts
    attachInterrupt(digitalPinToInterrupt(PIN_BTN_ABORT), &abortISR, FALLING);

    // TODO Read saved config from EEPROM
    setDefaultMasterConfig(&masterConfig);

    // Initialize RF24
    radio.begin();
    radio.setAutoAck(true);
    radio.setChannel(masterConfig.radioChannel);
    radio.setPALevel(RF24_PA_MAX);
    radio.setRetries(RF24_RETRIES_DELAY, RF24_RETRIES_COUNT);
    radio.openWritingPipe(masterConfig.targetID);
    radio.openReadingPipe(1, masterConfig.ownID);
    radio.stopListening();

    // Initialize SSD1306
    oled.begin(&Adafruit128x64, I2C_OLED_ADDRESS);
    delay(100);
    oled.setFont(TimesNewRoman16_bold);
    oled.clear();
    oled.println("detoNANO");
    oled.println("Welcome!");
    oled.println(isUnlocked ? "Locked" : "Unlocked");
    oled.println("Press OK");

#if DEBUG == true
    Serial.begin(BAUDRATE);
    printf_begin();
    Serial.println("**************** RF24 ****************");
    radio.printPrettyDetails();
    Serial.print("Is RF24 module connected?: ");
    Serial.println(radio.isChipConnected() ? "yes" : "no");
    Serial.println("**************** CONFIG ****************");
    printConfig(&masterConfig);
    Serial.println("**************** EEPROM ****************");
    printEEPROM();
    Serial.println("**************** DEBUG ****************");
#endif
}

void loop()
{

    if (btnCenter.isPressed())
    {
        if (screenIdx > STATE_COUNT_USER)
        {
            screenIdx = 0;
        }
        screenIdx++;
        switchScreenState(false, &oled, &btnCenter, &btnRight, &btnLeft, &isUnlocked, &screenIdx, &masterConfig, &slaveConfig);
    }

    if (isUnlocked)
    {
        uint8_t detbuf[1] = {10};
        if (digitalRead(PIN_DETONATE))
        {
            bool ack = radio.write(detbuf, 1);
            Serial.println("detonation packet sent");
            if (ack)
            {
                Serial.println("ack received");
            }
            else
            {
                Serial.println("ack not received");
            }
        }
    }
}

void abortISR()
{
    oled.setFont(TimesNewRoman16_bold);
    oled.clear();
    oled.println("ABORTED");
    oled.println("");
    oled.println("REBOOT");
    oled.println("DEVICE");
    while (1)
    {
    }
}