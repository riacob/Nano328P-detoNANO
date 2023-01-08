// 08 jan 2023
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

void setup()
{
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

    setDefaultSlaveConfig(&slaveConfig);
    writeConfig(&slaveConfig);
    readConfig(&slaveConfig);
    oled.begin(&Adafruit128x64, 0x3C);
    Serial.println("Receiver");
    radio.begin();
    radio.setChannel(slaveConfig.radioChannel);
    radio.setPALevel(RF24_PA_LEVEL);
    radio.openWritingPipe(slaveConfig.targetAddress);
    radio.openReadingPipe(1, slaveConfig.ownAddress);
    radio.startListening();
    
#if DEBUG == true
    Serial.begin(BAUDRATE);
    printf_begin();
    radio.printPrettyDetails();
    printConfig(&slaveConfig);
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
        Serial.print(dataReceived);
        oled.setFont(Adafruit5x7);
        oled.clear();
        oled.print(dataReceived);
        newData = false;
    }
}