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
#include "screenstates.h"
#include "debounce.h"
userconfig_s slaveConfig;

RF24 radio(PIN_RF24_CE, PIN_RF24_CSN);
SSD1306AsciiAvrI2c oled;
DebouncedButton btnCenter;
DebouncedButton btnLeft;
DebouncedButton btnRight;

uint8_t dataBuffer[32];
bool newData = false;
int screenIdx = 0;
uint8_t isUnlocked = 0;

void handlePackets();
void abortISR();
void radioISR();

void setup()
{
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
    pinMode(PIN_RELAY, OUTPUT);
    pinMode(PIN_SIREN, OUTPUT);

    // Initialize interrupts
    attachInterrupt(digitalPinToInterrupt(PIN_BTN_ABORT), &abortISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(PIN_RF24_IRQ), &radioISR, FALLING);

    // TODO Read saved config from EEPROM
    setDefaultSlaveConfig(&slaveConfig);

    // Initialize RF24
    radiook = radio.begin();
    radio.maskIRQ(true, true, false);
    radio.setAutoAck(true);
    radio.setChannel(slaveConfig.radioChannel);
    radio.setPALevel(RF24_PA_LEVEL);
    radio.setDataRate(RF24_DATARATE);
    radio.setRetries(RF24_RETRIES_DELAY, RF24_RETRIES_COUNT);
    radio.openWritingPipe(slaveConfig.targetID);
    radio.openReadingPipe(1, slaveConfig.ownID);
    radio.startListening();

    // Initialize SSD1306
    oled.begin(&Adafruit128x64, I2C_OLED_ADDRESS);
    delay(100);
    switchScreenState(false, &oled, &btnCenter, &btnRight, &btnLeft, &isUnlocked, &screenIdx, &slaveConfig);

#if DEBUG == true
    Serial.begin(BAUDRATE);
    printf_begin();
    Serial.println("**************** RF24 ****************");
    radio.printPrettyDetails();
    Serial.print("Is RF24 module connected?: ");
    Serial.println(radio.isChipConnected() ? "yes" : "no");
    Serial.println("**************** CONFIG ****************");
    printConfig(&slaveConfig);
    Serial.println("**************** EEPROM ****************");
    printEEPROM();
    Serial.println("**************** DEBUG ****************");
#endif

    // Make sure RF24 is okay
    if (!radiook)
    {
        oled.setFont(TimesNewRoman16_bold);
        oled.clear();
        oled.println("ERROR");
        oled.println("");
        oled.println("FAULTY");
        oled.println("RADIO");
        Serial.println("ERROR RADIO MODULE IS MISSING");
        while (1)
        {
        }
    }
}

void loop()
{
    handlePackets();
    if (btnCenter.isPressed())
    {
        screenIdx++;
        switchScreenState(false, &oled, &btnCenter, &btnRight, &btnLeft, &isUnlocked, &screenIdx, &slaveConfig);
    }
}

void handlePackets()
{
    if (!newData)
    {
        return;
    }
    if (!isUnlocked)
    {
        return;
    }
    // Switch between commands
    switch (dataBuffer[0])
    {
    case CMD_ABORT:
    {
        abortISR();
        break;
    }
    case CMD_DETONATE:
    {
        // If device is disarmed, flush packet and return
        if (!digitalRead(PIN_ARM))
        {
            radio.flush_rx();
            digitalWrite(PIN_RELAY, LOW);
            break;
        }
        // Else detonate
        digitalWrite(PIN_LED_B, HIGH);
        delay(slaveConfig.detonationPulseTime);
        digitalWrite(PIN_LED_B, LOW);
        break;
    }
    case CMD_CONFIG:
    {
        receiveConfigFromMaster(&slaveConfig, dataBuffer);
        break;
    }
    case CMD_WATCHDOG:
    {
    }
    }
    newData = false;
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

void radioISR()
{
    if (!isUnlocked)
    {
        // Flush any already-present packets, for safety on the first unlock
        radio.flush_rx();
        return;
    }
    bool tx_ok, tx_fail, rx_ready;
    radio.whatHappened(tx_ok, tx_fail, rx_ready);
    if (radio.available())
    {
        radio.read(&dataBuffer, 32);
        newData = true;
    }
}