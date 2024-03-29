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
#include "debug.h"

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
    Serial.begin(BAUDRATE);
    bool radiook = false;
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
    setDefaultSlaveConfig(&slaveConfig);

    // Initialize RF24
    radiook = radio.begin();
    radio.setAutoAck(true);
    radio.setChannel(masterConfig.radioChannel);
    radio.setPALevel(RF24_PA_LEVEL);
    radio.setDataRate(RF24_DATARATE);
    radio.setRetries(RF24_RETRIES_DELAY, RF24_RETRIES_COUNT);
    radio.openWritingPipe(masterConfig.targetID);
    radio.openReadingPipe(1, masterConfig.ownID);
    radio.stopListening();

    // Initialize SSD1306
    oled.begin(&Adafruit128x64, I2C_OLED_ADDRESS);
    delay(100);
    switchScreenState(false, &oled, &btnCenter, &btnRight, &btnLeft, &isUnlocked, &screenIdx, &masterConfig, &slaveConfig);

    printf_begin();
    debugln("**************** RF24 ****************");
    radio.printPrettyDetails();
    debug("Is RF24 module connected?: ");
    debugln(radio.isChipConnected() ? "yes" : "no");
    debugln("**************** CONFIG ****************");
    printConfig(&masterConfig);
    debugln("**************** EEPROM ****************");
    printEEPROM();
    debugln("**************** DEBUG ****************");

    // Make sure RF24 is okay
    if (!radiook)
    {
        oled.setFont(TimesNewRoman16_bold);
        oled.clear();
        oled.println("ERROR");
        oled.println("");
        oled.println("FAULTY");
        oled.println("RADIO");
        debugln("ERROR RADIO MODULE IS MISSING");
        while (1)
        {
        }
    }
}

void loop()
{
    if (btnCenter.isPressed())
    {
        screenIdx++;
        switchScreenState(false, &oled, &btnCenter, &btnRight, &btnLeft, &isUnlocked, &screenIdx, &masterConfig, &slaveConfig);
    }

    if (isUnlocked)
    {
        uint8_t detbuf[1] = {CMD_DETONATE};
        if (digitalRead(PIN_DETONATE))
        {
            radio.write(detbuf, 1);
            delay(20);
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
    uint8_t buf[1] = {CMD_ABORT};
    while (1)
    {
        radio.write(buf, 1);
    }
}