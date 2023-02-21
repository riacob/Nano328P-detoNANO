/**
 * @file main_rx.cpp
 * @author Riccardo Iacob
 * @brief Main file for the receiver
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
userconfig_s slaveConfig;

RF24 radio(PIN_RF24_CE, PIN_RF24_CSN);
SSD1306AsciiAvrI2c oled;

char dataReceived[32];
bool newData = false;

void getData();
void showData();
void abortISR();

void setup()
{
    // Initialize GPIOs
    pinMode(PIN_LED_R, OUTPUT);
    pinMode(PIN_LED_G, OUTPUT);
    pinMode(PIN_LED_B, OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(PIN_BTN_ABORT, INPUT_PULLUP);
    pinMode(PIN_BTN_CENTER, INPUT_PULLUP);
    pinMode(PIN_BTN_RIGHT, INPUT_PULLUP);
    pinMode(PIN_BTN_LEFT, INPUT_PULLUP);
    pinMode(PIN_RF24_IRQ, INPUT);
    pinMode(PIN_RF24_CE, OUTPUT);
    pinMode(PIN_RF24_CSN, OUTPUT);
    pinMode(PIN_ARM, INPUT);
    pinMode(PIN_DETONATE, INPUT);
    pinMode(PIN_RELAY, OUTPUT);
    pinMode(PIN_SIREN, OUTPUT);

    // Initialize interrupts
    attachInterrupt(digitalPinToInterrupt(PIN_BTN_ABORT), &abortISR, FALLING);

    // TODO Read saved config from EEPROM
    setDefaultSlaveConfig(&slaveConfig);

    // Initialize RF24
    radio.begin();
    radio.setChannel(slaveConfig.radioChannel);
    radio.setPALevel(RF24_PA_MIN);
    radio.setRetries(RF24_RETRIES_DELAY, RF24_RETRIES_COUNT);
    radio.openWritingPipe(slaveConfig.targetID);
    radio.openReadingPipe(1, slaveConfig.ownID);
    radio.startListening();

    // Initialize SSD1306
    oled.begin(&Adafruit128x64, I2C_OLED_ADDRESS);
    delay(100);
    oled.setFont(TimesNewRoman16_bold);
    oled.clear();
    oled.println("detoNANO");
    oled.println("Welcome!");
    oled.println();
    oled.println("Press OK");

#if DEBUG == true
    Serial.begin(BAUDRATE);
    printf_begin();
    Serial.println("**************** RF24 ****************");
    radio.printPrettyDetails();
    Serial.println("**************** CONFIG ****************");
    printConfig(&slaveConfig);
    Serial.println("**************** EEPROM ****************");
    printEEPROM();
    Serial.println("**************** DEBUG ****************");
#endif
}

void loop()
{
    getData();
    showData();
}

void getData()
{
    if (radio.available())
    {
        radio.read(&dataReceived, 32);
        newData = true;
    }
}

void showData()
{
    if (newData == true)
    {
        Serial.print("Data received ");
        for (int i = 0; i < 32; i++)
        {
            Serial.print((int)dataReceived[i]);
            Serial.print(" - ");
            Serial.print((char)dataReceived[i]);
            Serial.print(",");
        }
        Serial.println();
        oled.setFont(Adafruit5x7);
        oled.clear();
        oled.print(dataReceived);
        if (dataReceived[0] == 10)
        {
            digitalWrite(PIN_RELAY, HIGH);
            delay(250);
            digitalWrite(PIN_RELAY, LOW);
        }
        newData = false;
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