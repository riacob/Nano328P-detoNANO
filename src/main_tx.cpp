/**
 * @file main_tx.cpp
 * @author Riccardo Iacob
 * @brief
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

RF24 radio(PIN_RF24_CE, PIN_RF24_CSN);
SSD1306AsciiAvrI2c oled;
DebouncedButton btnCenter;
DebouncedButton btnLeft;
DebouncedButton btnRight;
userconfig_s masterConfig;
userconfig_s slaveConfig;

uint8_t packetBuffer[32] = "Hello from nRF24!";
int ctr_btn = 0;
bool deviceUnlocked = 0;

uint8_t digitalReadDebounce(uint8_t digitalPin);
void switchScreen(int *screenIdx, userconfig_s *config, uint8_t isReceiver = 0);

void setup()
{
    pinMode(PIN_LED_R, OUTPUT);
    pinMode(PIN_LED_G, OUTPUT);
    pinMode(PIN_LED_B, OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(PIN_BTN_ABORT, INPUT_PULLUP);
    // pinMode(PIN_BTN_CENTER, INPUT_PULLUP);
    // pinMode(PIN_BTN_RIGHT, INPUT_PULLUP);
    // pinMode(PIN_BTN_LEFT, INPUT_PULLUP);
    btnCenter.begin(PIN_BTN_CENTER);
    btnLeft.begin(PIN_BTN_LEFT);
    btnRight.begin(PIN_BTN_RIGHT);
    pinMode(PIN_RF24_IRQ, INPUT);
    pinMode(PIN_RF24_CE, OUTPUT);
    pinMode(PIN_RF24_CSN, OUTPUT);
    pinMode(PIN_ARM, INPUT);
    pinMode(PIN_DETONATE, INPUT);

    // readConfig(&masterConfig);
    setDefaultMasterConfig(&masterConfig);
    setDefaultSlaveConfig(&slaveConfig);

    radio.begin();
    radio.setChannel(masterConfig.radioChannel);
    radio.setPALevel(RF24_PA_LEVEL);
    radio.setRetries(RF24_RETRIES_DELAY, RF24_RETRIES_COUNT);
    radio.openWritingPipe(masterConfig.targetID);
    radio.openReadingPipe(1, masterConfig.ownID);
    radio.stopListening();

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
    Serial.println("****************  RF24  ****************");
    radio.printPrettyDetails();
    Serial.println("**************** CONFIG ****************");
    printConfig(&masterConfig);
    Serial.println("**************** EEPROM ****************");
    printEEPROM();
    transmitConfigToSlave(&slaveConfig, &radio);
#endif
}

void loop()
{
    if (btnCenter.isPressed())
    {
        ctr_btn++;
        switchScreen(&ctr_btn, &masterConfig);
    }
}

void switchScreen(int *screenIdx, userconfig_s *config, uint8_t deviceMode)
{
    // Reset to main screen when we reach the system-access-only screens
    if (*screenIdx > MENU_ITEMS)
    {
        *screenIdx = 0;
    }
    // If pin is enabled, ask for pin
    if ((config->pinEnabled && (*screenIdx > 0)) || deviceUnlocked)
    {
        // Number of times the OK button has been pressed
        // When it was pressed 4 times, all digits were entered, check pin
        uint8_t okPresses = 0;
        uint8_t pin[4] = {0};
        while (okPresses < 4)
        {
            oled.setFont(font5x7);
            oled.clear();
            oled.println("Device Locked");
            oled.println("Enter PIN");
            oled.println();
            oled.print("[ ");
            oled.print(pin[0]);
            oled.print(" - ");
            oled.print(pin[1]);
            oled.print(" - ");
            oled.print(pin[2]);
            oled.print(" - ");
            oled.print(pin[3]);
            oled.print(" ]");
            // Digits go from 0 to 9
            // If value is >9, roll over to 0
            // If value is <0, roll over to 9
            // Editing first digit
            if (okPresses == 0)
            {
                if (btnRight.isPressed())
                {
                    if (pin[0] == 9)
                    {
                        pin[0] = 0;
                    }
                    pin[0]++;
                }
                if (btnLeft.isPressed())
                {
                    if (pin[0] == 0)
                    {
                        pin[0] = 9;
                    }
                    pin[0]--;
                }
            }
            // Editing second digit
            if (okPresses == 1)
            {
                if (btnRight.isPressed())
                {
                    if (pin[1] == 9)
                    {
                        pin[1] = 0;
                    }
                    pin[1]++;
                }
                if (btnLeft.isPressed())
                {
                    if (pin[1] == 0)
                    {
                        pin[1] = 9;
                    }
                    pin[1]--;
                }
            }
            // Editing third digit
            if (okPresses == 2)
            {
                if (btnRight.isPressed())
                {
                    if (pin[2] == 9)
                    {
                        pin[2] = 0;
                    }
                    pin[2]++;
                }
                if (btnLeft.isPressed())
                {
                    if (pin[2] == 0)
                    {
                        pin[2] = 9;
                    }
                    pin[2]--;
                }
            }
            // Editing fourth digit
            if (okPresses == 3)
            {
                if (btnRight.isPressed())
                {
                    if (pin[3] == 9)
                    {
                        pin[3] = 0;
                    }
                    pin[3]++;
                }
                if (btnLeft.isPressed())
                {
                    if (pin[3] == 0)
                    {
                        pin[3] = 9;
                    }
                    pin[3]--;
                }
            }
        }
        // Compare PINs
        for (int i = 0; i < 4; i++)
        {
            // If a digit of the pin is not correct, restart the pin entering process
            if (pin[i] != config->pin[i])
            {
                deviceUnlocked = 0;
                switchScreen(screenIdx, config);
            }
            // If pin is correct, continue
            else
            {
                deviceUnlocked = 1;
                switchScreen(screenIdx, config);
            }
        }
#if DEBUG == true
        int a;
        Serial.print("Entered PIN: ");
        for (a = 0; a < 4; a++)
        {
            Serial.print((int)pin[a]);
            Serial.print("-");
        }
        Serial.println();
        Serial.print("Correct PIN: ");
        for (a = 0; a < 4; a++)
        {
            Serial.print((int)config->pin[a]);
            Serial.print("-");
        }
        Serial.println();
        Serial.print("isCorrect: ");
        Serial.println(deviceUnlocked);
#endif
    }
    switch (*screenIdx)
    {
    // Print home screen
    case 0:
    {
        oled.setFont(font5x7);
        oled.clear();
        oled.println("detoNANO");
        oled.println();
        oled.println();
        oled.println("Press OK");
        break;
    }
    // Print button tutorial
    case 1:
    {
        oled.setFont(font5x7);
        oled.clear();
        oled.println("MENU TUTORIAL");
        oled.println("Center: OK/ENTER");
        oled.println("Right: UP");
        oled.println("Left: DOWN");
        break;
    }
    // Print device configuration
    case 2:
    {
        oled.setFont(font5x7);
        oled.clear();
        oled.println("CONFIGURATION");
        oled.print("Channel: ");
        oled.println((int)config->radioChannel);
        oled.print("targID: ");
        for (int i = 0; i < 5; i++)
        {
            oled.print((int)config->targetID[i]);
            oled.print(" ");
        }
        oled.println();
        oled.print("ownID: ");
        for (int i = 0; i < 5; i++)
        {
            oled.print((int)config->ownID[i]);
            oled.print(" ");
        }
        oled.println();
        oled.print("detDelay: ");
        oled.println(config->detonationDelay);
        break;
    }
    // COMMON SETTINGS
    // Edit detonation delay
    case 3:
    {
        uint8_t okPressed = 0;
        oled.setFont(font5x7);
        oled.clear();
        oled.println("Set detonationDelay");
        oled.print("Current value: ");
        oled.println(masterConfig.detonationDelay);
        oled.println("Press OK to save");
        while (!okPressed)
        {
            if (btnRight.isPressed())
            {
                if (masterConfig.detonationDelay == UINT32_MAX)
                {
                    masterConfig.detonationDelay = 0;
                    slaveConfig.detonationDelay = 0;
                }
                masterConfig.detonationDelay += 5000;
                slaveConfig.detonationDelay += 5000;
                oled.clearField(0, 4, 5);
                oled.print(masterConfig.detonationDelay);
            }
            if (btnLeft.isPressed())
            {
                if (masterConfig.detonationDelay <= 0)
                {
                    masterConfig.detonationDelay = 0;
                    slaveConfig.detonationDelay = 0;
                }
                masterConfig.detonationDelay -= 5000;
                slaveConfig.detonationDelay -= 5000;
                oled.clearField(0, 4, 5);
                oled.print(masterConfig.detonationDelay);
            }
            okPressed = btnCenter.isPressed();
        }
        break;
    }
    // TRANSMITTER ONLY SETTINGS
    // Edit tx radio channel
    case 4:
    {
        uint8_t okPressed = 0;
        oled.setFont(font5x7);
        oled.clear();
        oled.println("Set TX radioChannel");
        oled.print("Current value: ");
        oled.println(masterConfig.radioChannel);
        oled.println("Press OK to save");
        while (!okPressed)
        {
            if (btnRight.isPressed())
            {
                if (masterConfig.radioChannel == 126)
                {
                    masterConfig.radioChannel = 0;
                }
                masterConfig.radioChannel++;
                oled.clearField(0, 4, 5);
                oled.print(masterConfig.radioChannel);
            }
            if (btnLeft.isPressed())
            {
                if (masterConfig.radioChannel <= 0)
                {
                    masterConfig.radioChannel = 0;
                }
                masterConfig.radioChannel--;
                oled.clearField(0, 4, 5);
                oled.print(masterConfig.radioChannel);
            }
            okPressed = btnCenter.isPressed();
        }
        break;
    }
    // RECEIVER ONLY SETTINGS
    }
}