// 8 jan 2023
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

uint8_t digitalReadDebounce(uint8_t digitalPin);
void switchScreen(int *screenIdx);

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

    readConfig(&masterConfig);

    radio.begin();
    radio.setChannel(masterConfig.radioChannel);
    radio.setPALevel(RF24_PA_LEVEL);
    radio.setRetries(RF24_RETRIES_DELAY, RF24_RETRIES_COUNT);
    radio.openWritingPipe(masterConfig.targetAddress);
    radio.openReadingPipe(1, masterConfig.ownAddress);
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
    Serial.println("******** RF24 ********");
    radio.printPrettyDetails();
    Serial.println("******** CONFIG ********");
    printConfig(&masterConfig);
    Serial.println("******** EEPROM ********");
    printEEPROM();
#endif
}

void loop()
{
    if (btnCenter.isPressed())
    {
        ctr_btn++;
        switchScreen(&ctr_btn);
    }
}

void switchScreen(int *screenIdx)
{
    // Reset to main screen when we reach the system-access-only screens
    if (*screenIdx > MENU_ITEMS)
    {
        *screenIdx = 0;
    }
    switch (*screenIdx)
    {
        // Home screen
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
    // Button tutorial
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
    // Transmitter configuration
    case 2:
    {
        oled.setFont(font5x7);
        oled.clear();
        oled.println("TX CONFIGURATION");
        oled.print("Channel: ");
        oled.println((int)masterConfig.radioChannel);
        oled.print("targAdd: ");
        for (int i = 0; i < 5; i++)
        {
            oled.print((int)masterConfig.targetAddress[i]);
            oled.print(" ");
        }
        oled.println();
        oled.print("ownAdd: ");
        for (int i = 0; i < 5; i++)
        {
            oled.print((int)masterConfig.ownAddress[i]);
            oled.print(" ");
        }
        oled.println();
        oled.print("detDelay: ");
        oled.println(masterConfig.detonationDelay);
        break;
    }
    // Receiver configuration
    case 3:
    {
        oled.setFont(font5x7);
        oled.clear();
        oled.println("RX CONFIGURATION");
        oled.print("Channel: ");
        oled.println((int)slaveConfig.radioChannel);
        oled.print("targAdd: ");
        for (int i = 0; i < 5; i++)
        {
            oled.print((int)slaveConfig.targetAddress[i]);
            oled.print(" ");
        }
        oled.println();
        oled.print("ownAdd: ");
        for (int i = 0; i < 5; i++)
        {
            oled.print((int)slaveConfig.ownAddress[i]);
            oled.print(" ");
        }
        oled.println();
        oled.print("detDelay: ");
        oled.println(slaveConfig.detonationDelay);
        break;
    }
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
    // Edit tx detonation delay
    case 5:
    {
        uint8_t okPressed = 0;
        oled.setFont(font5x7);
        oled.clear();
        oled.println("Set TX detonationDelay");
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
                }
                masterConfig.detonationDelay += 5000;
                oled.clearField(0, 4, 5);
                oled.print(masterConfig.detonationDelay);
            }
            if (btnLeft.isPressed())
            {
                if (masterConfig.detonationDelay <= 0)
                {
                    masterConfig.detonationDelay = 0;
                }
                masterConfig.detonationDelay--;
                oled.clearField(0, 4, 5);
                oled.print(masterConfig.detonationDelay);
            }
            okPressed = btnCenter.isPressed();
        }
        break;
    }
    }
}