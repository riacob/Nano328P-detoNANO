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

/**
 * @brief All possible OLED screen states
 * 
 * @note STATE_COUNT_USER and STATE_COUNT_AUTO shall always be at the bottom of the two lists
 * @note Changing the order of the "user states" (first list) will change the order they will be displayed in on the OLED screen
 */
enum SCREEN_STATES
{
    // States accessible by the user
    // For example home screen, setting screen...
    STATE_PRINT_HOME_SCREEN,
    STATE_PRINT_TUTORIAL,
    STATE_PRINT_DEVICE_CONFIGURATION,
    STATE_EDIT_COMMON_DETONATIONDELAY,
    STATE_EDIT_TX_RADIOCHANNEL,
    STATE_COUNT_USER,

    // States only accessible by the system
    // All status messages, for example "detonating in xxx mS"
    STATE_COUNT_AUTO
};

RF24 radio(PIN_RF24_CE, PIN_RF24_CSN);
SSD1306AsciiAvrI2c oled;
DebouncedButton btnCenter;
DebouncedButton btnLeft;
DebouncedButton btnRight;
userconfig_s masterConfig;
userconfig_s slaveConfig;

// The data buffer to be sent trough radio
uint8_t packetBuffer[32] = "Hello from nRF24!";
// The number of times the center button has been pressed
int ctr_btn = 0;
// Wether or not the device is unlocked
bool deviceUnlocked = 0;

uint8_t digitalReadDebounce(uint8_t digitalPin);
void switchScreenState(int *screenIdx, userconfig_s *config);

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

    // Read saved config from EEPROM
    readConfig(&masterConfig);
    //setDefaultMasterConfig(&masterConfig);
    //setDefaultSlaveConfig(&slaveConfig);
    //writeConfig(&masterConfig);

    // Initialize RF24
    radio.begin();
    radio.setChannel(masterConfig.radioChannel);
    radio.setPALevel(RF24_PA_LEVEL);
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
    oled.println();
    oled.println("Press OK");

#if DEBUG == true
    Serial.begin(BAUDRATE);
    printf_begin();
    Serial.println("**************** RF24 ****************");
    radio.printPrettyDetails();
    Serial.println("**************** CONFIG ****************");
    printConfig(&masterConfig);
    //Serial.println("**************** EEPROM ****************");
    //printEEPROM();
    Serial.println("**************** DEBUG ****************");
    transmitConfigToSlave(&slaveConfig, &radio);
    uint8_t rbfr[32] = {'C','{','"','r','c','"',':','7','6','}'};
    receiveConfigFromMaster(&slaveConfig, rbfr);
#endif
}

void loop()
{
    if (btnCenter.isPressed())
    {
        if (ctr_btn > STATE_COUNT_USER) {
            ctr_btn = 0;
        }
        ctr_btn++;
        switchScreenState(&ctr_btn, &masterConfig);
    }
}

void switchScreenState(int *screenIdx, userconfig_s *config)
{
    // If screenIdx has a value that doesn't exist in the SCREEN_STATE enum, set it to zero
    if (*screenIdx > STATE_COUNT_AUTO)
    {
        *screenIdx = STATE_PRINT_HOME_SCREEN;
    }
    // If pin is not enabled, unlock device
    if (!config->pinEnabled) {
        deviceUnlocked = 1;
    }
    // If pin is enabled, ask for pin
    if ((config->pinEnabled && (*screenIdx > 0)) && !deviceUnlocked)
    {
        // Number of times the OK button has been pressed
        // When it was pressed 4 times, all digits were entered, check pin
        uint8_t okPresses = 0;
        // PIN entered by the user
        uint8_t pin[4] = {0};
        // Update OLED only when something changed
        uint8_t oledNeedsUpdate = 1;
        while (okPresses < 4)
        {
            if (oledNeedsUpdate)
            {
                oled.setFont(font5x7);
                oled.clear();
                oled.println("    Device Locked");
                oled.println("      Enter PIN");
                oled.println();
                oled.print("  [ ");
                oled.print(pin[0]);
                oled.print(" - ");
                oled.print(pin[1]);
                oled.print(" - ");
                oled.print(pin[2]);
                oled.print(" - ");
                oled.print(pin[3]);
                oled.println(" ]");
                // Print cursor under the digit the user is currently editing
                // [ 0 - 0 - 0 - 0 ]
                //   -
                if (okPresses == 0)
                {
                    oled.println("    -");
                }
                if (okPresses == 1)
                {
                    oled.println("        -");
                }
                if (okPresses == 2)
                {
                    oled.println("            -");
                }
                if (okPresses == 3)
                {
                    oled.println("                -");
                }
                oled.println();
                oled.println(" OK to change digit");
                oled.println("UP/DOWN to change val");
                oledNeedsUpdate = 0;
            }
            // If the OK button is pressed, increment the number of times it has been pressed
            if (btnCenter.isPressed())
            {
                okPresses++;
                // OLED // OLED contents have changed, so update it have changed, so update it
                oledNeedsUpdate = 1;
            }
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
                    else
                    {
                        pin[0]++;
                    }
                    // OLED contenents have changed, so update it
                    oledNeedsUpdate = 1;
                }
                if (btnLeft.isPressed())
                {
                    if (pin[0] == 0)
                    {
                        pin[0] = 9;
                    }
                    else
                    {
                        pin[0]--;
                    }
                    // OLED contenents have changed, so update it
                    oledNeedsUpdate = 1;
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
                    else
                    {
                        pin[1]++;
                    }
                    // OLED contenents have changed, so update it
                    oledNeedsUpdate = 1;
                }
                if (btnLeft.isPressed())
                {
                    if (pin[1] == 0)
                    {
                        pin[1] = 9;
                    }
                    else
                    {
                        pin[1]--;
                    }
                    // OLED contenents have changed, so update it
                    oledNeedsUpdate = 1;
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
                    else
                    {
                        pin[2]++;
                    }
                    // OLED contenents have changed, so update it
                    oledNeedsUpdate = 1;
                }
                if (btnLeft.isPressed())
                {
                    if (pin[2] == 0)
                    {
                        pin[2] = 9;
                    }
                    else
                    {
                        pin[2]--;
                    }
                    // OLED contenents have changed, so update it
                    oledNeedsUpdate = 1;
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
                    else
                    {
                        pin[3]++;
                    }
                    // OLED contenents have changed, so update it
                    oledNeedsUpdate = 1;
                }
                if (btnLeft.isPressed())
                {
                    if (pin[3] == 0)
                    {
                        pin[3] = 9;
                    }
                    else
                    {
                        pin[3]--;
                    }
                    // OLED contenents have changed, so update it
                    oledNeedsUpdate = 1;
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
                *screenIdx = STATE_PRINT_HOME_SCREEN;
                // OLED contenents have changed, so update it
                oled.setFont(font5x7);
                oled.clear();
                oled.println("    Device Locked");
                oled.println("      Enter PIN");
                oled.println();
                oled.println("      Wrong PIN");
                oled.println("  Press OK to retry");
                oled.println();
                oled.println(" OK to change digit");
                oled.println("UP/DOWN to change val");
                return;
            }
        }
        // If pin is correct, continue
        deviceUnlocked = 1;
        *screenIdx = STATE_PRINT_HOME_SCREEN;
    }
    switch (*screenIdx)
    {
    // Print home screen
    case STATE_PRINT_HOME_SCREEN:
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
    case STATE_PRINT_TUTORIAL:
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
    case STATE_PRINT_DEVICE_CONFIGURATION:
    {
        oled.setFont(font5x7);
        oled.clear();
        oled.println("CONFIGURATION");
        oled.print("Role: ");
        if (config->ownID[0] == 'M') {
            oled.println("TRANSMITTER");
        } else {
            oled.println("RECEIVER");
        }
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
    case STATE_EDIT_COMMON_DETONATIONDELAY:
    {
        uint8_t okPressed = 0;
        oled.setFont(font5x7);
        oled.clear();
        oled.println("Set detonationDelay");
        oled.print("Current value: ");
        oled.println(config->detonationDelay);
        oled.println("Press OK to save");
        while (!okPressed)
        {
            if (btnRight.isPressed())
            {
                if (config->detonationDelay == UINT32_MAX)
                {
                    config->detonationDelay = 0;
                }
                config->detonationDelay += 5000;
                oled.clearField(0, 20, 5);
                oled.print(config->detonationDelay);
            }
            if (btnLeft.isPressed())
            {
                if (config->detonationDelay <= 0)
                {
                    config->detonationDelay = 0;
                }
                config->detonationDelay -= 5000;
                oled.clearField(0, 20, 5);
                oled.print(config->detonationDelay);
            }
            okPressed = btnCenter.isPressed();
        }
        break;
    }
    // TRANSMITTER ONLY SETTINGS
    // Edit tx radio channel
    case STATE_EDIT_TX_RADIOCHANNEL:
    {
        // If the current device is not a transmitter, move on
        if (config->ownID[0] != 'M') {
            break;
        }
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