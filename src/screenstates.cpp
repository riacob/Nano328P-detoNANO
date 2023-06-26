#include "screenstates.h"

void switchScreenState(bool isSystemScreen, SSD1306AsciiAvrI2c *oled, DebouncedButton *btnCenter, DebouncedButton *btnRight, DebouncedButton *btnLeft, uint8_t *isUnlocked, int *screenIdx, userconfig_s *config, userconfig_s *slaveConfig)
{
    // If screenIdx has a value that doesn't exist in the SCREEN_STATE enum, set it to zero
    if ((isSystemScreen && (*screenIdx == STATE_COUNT_TOT)) || (!isSystemScreen && (*screenIdx == STATE_COUNT_USER)))
    {
        *screenIdx = STATE_USR_PRINT_HOME_SCREEN;
    }
    // If pin is not enabled, unlock device
    if (!config->pinEnabled)
    {
        *isUnlocked = 1;
    }
    // If pin is enabled, ask for pin
    if ((config->pinEnabled && (*screenIdx > 0)) && !(*isUnlocked))
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
                oled->setFont(font5x7);
                oled->clear();
                oled->println("    Device Locked");
                oled->println("      Enter PIN");
                oled->println();
                oled->print("  [ ");
                oled->print(pin[0]);
                oled->print(" - ");
                oled->print(pin[1]);
                oled->print(" - ");
                oled->print(pin[2]);
                oled->print(" - ");
                oled->print(pin[3]);
                oled->println(" ]");
                // Print cursor under the digit the user is currently editing
                // [ 0 - 0 - 0 - 0 ]
                //   -
                if (okPresses == 0)
                {
                    oled->println("    -");
                }
                if (okPresses == 1)
                {
                    oled->println("        -");
                }
                if (okPresses == 2)
                {
                    oled->println("            -");
                }
                if (okPresses == 3)
                {
                    oled->println("                -");
                }
                oled->println();
                oled->println(" OK to change digit");
                oled->println("UP/DOWN to change val");
                oledNeedsUpdate = 0;
            }
            // If the OK button is pressed, increment the number of times it has been pressed
            if (btnCenter->isPressed())
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
                if (btnRight->isPressed())
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
                if (btnLeft->isPressed())
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
                if (btnRight->isPressed())
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
                if (btnLeft->isPressed())
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
                if (btnRight->isPressed())
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
                if (btnLeft->isPressed())
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
                if (btnRight->isPressed())
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
                if (btnLeft->isPressed())
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
                *isUnlocked = 0;
                *screenIdx = STATE_USR_PRINT_HOME_SCREEN;
                // OLED contenents have changed, so update it
                oled->setFont(font5x7);
                oled->clear();
                oled->println("    Device Locked");
                oled->println("      Enter PIN");
                oled->println();
                oled->println("      Wrong PIN");
                oled->println("  Press OK to retry");
                oled->println();
                oled->println(" OK to change digit");
                oled->println("UP/DOWN to change val");
                return;
            }
        }
        // If pin is correct, continue
        *isUnlocked = 1;
        *screenIdx = STATE_USR_PRINT_HOME_SCREEN;
    }
    switch (*screenIdx)
    {
    // Print home screen
    case STATE_USR_PRINT_HOME_SCREEN:
    {
        oled->setFont(font5x7);
        oled->clear();
        oled->println("detoNANO");
        oled->println("Welcome!");
        oled->println(*isUnlocked ? "Unlocked" : "Locked");
        oled->println("Press OK");
        break;
    }
    // Print button tutorial
    case STATE_USR_PRINT_TUTORIAL:
    {
        oled->setFont(font5x7);
        oled->clear();
        oled->println("MENU TUTORIAL");
        oled->println("Center: OK/ENTER");
        oled->println("Right: UP");
        oled->println("Left: DOWN");
        break;
    }
    // Print device configuration
    case STATE_USR_PRINT_DEVICE_CONFIGURATION_1:
    {
        oled->setFont(font5x7);
        oled->clear();
        oled->println("CONFIG PAGE 1/1");
        oled->print("Role: ");
        if (config->ownID[0] == 'M')
        {
            oled->println("TRANSMITTER");
        }
        else
        {
            oled->println("RECEIVER");
        }
        oled->print("Channel: ");
        oled->println((int)config->radioChannel);
        oled->print("targID: ");
        for (int i = 0; i < 5; i++)
        {
            oled->print((int)config->targetID[i]);
            oled->print(" ");
        }
        oled->println();
        oled->print("ownID: ");
        for (int i = 0; i < 5; i++)
        {
            oled->print((int)config->ownID[i]);
            oled->print(" ");
        }
        oled->println();
        oled->print("detDelay: ");
        oled->println(config->detonationDelay);
        oled->print("detPulseTime: ");
        oled->println(config->detonationPulseTime);
        break;
    }
    // COMMON SETTINGS
    // Edit detonation delay
    case STATE_USR_EDIT_COMMON_DETONATIONDELAY:
    {
        uint8_t okPressed = 0;
        oled->setFont(font5x7);
        oled->clear();
        oled->println("Set detDelay");
        oled->print("Current value: ");
        oled->println(config->detonationDelay);
        oled->println("Press OK to save");
        while (!okPressed)
        {
            if (btnRight->isPressed())
            {
                config->detonationDelay += 5000;
                if (config->detonationDelay > 3600000)
                {
                    config->detonationDelay = 0;
                }
                oled->clearField(0, 3, 20);
                oled->print(config->detonationDelay);
            }
            if (btnLeft->isPressed())
            {
                config->detonationDelay -= 5000;
                if (config->detonationDelay > 3600000)
                {
                    config->detonationDelay = 0;
                }
                oled->clearField(0, 3, 20);
                oled->print(config->detonationDelay);
            }
            okPressed = btnCenter->isPressed();
        }
        break;
    }
    // Edit detonation pulse time
    case STATE_USR_EDIT_COMMON_DETONATIONPULSETIME:
    {
        uint8_t okPressed = 0;
        oled->setFont(font5x7);
        oled->clear();
        oled->println("Set detPulseTime");
        oled->print("Current value: ");
        oled->println(config->detonationPulseTime);
        oled->println("Press OK to save");
        while (!okPressed)
        {
            if (btnRight->isPressed())
            {
                config->detonationPulseTime += 50;
                if (config->detonationPulseTime > 3600000)
                {
                    config->detonationPulseTime = 0;
                }
                oled->clearField(0, 3, 20);
                oled->print(config->detonationPulseTime);
            }
            if (btnLeft->isPressed())
            {
                config->detonationPulseTime -= 50;
                if (config->detonationPulseTime > 3600000)
                {
                    config->detonationPulseTime = 0;
                }
                oled->clearField(0, 3, 20);
                oled->print(config->detonationPulseTime);
            }
            okPressed = btnCenter->isPressed();
        }
        break;
    }
    // TRANSMITTER ONLY SETTINGS
    // Edit tx radio channel
    case STATE_USR_EDIT_TX_RADIOCHANNEL:
    {
        // If the current device is not a transmitter, move on
        if (config->ownID[0] != 'M')
        {
            break;
        }
        uint8_t okPressed = 0;
        oled->setFont(font5x7);
        oled->clear();
        oled->println("Set TX radioChannel");
        oled->print("Current value: ");
        oled->println(config->radioChannel);
        oled->println("Press OK to save");
        while (!okPressed)
        {
            if (btnRight->isPressed())
            {
                config->radioChannel++;
                if (config->radioChannel > 127)
                {
                    config->radioChannel = 0;
                }
                // Second byte of the ID is always the radio channel
                config->ownID[1] = config->radioChannel;
                config->targetID[1] = config->radioChannel;
                oled->clearField(0, 3, 20);
                oled->print(config->radioChannel);
            }
            if (btnLeft->isPressed())
            {
                config->radioChannel--;
                if (config->radioChannel > 127)
                {
                    config->radioChannel = 0;
                }
                // Second byte of the ID is always the radio channel
                config->ownID[1] = config->radioChannel;
                config->targetID[1] = config->radioChannel;
                oled->clearField(0, 3, 20);
                oled->print(config->radioChannel);
            }
            okPressed = btnCenter->isPressed();
        }
        break;
    }
        // RECEIVER ONLY SETTINGS
        // SYSTEM SCREENS
    }
}