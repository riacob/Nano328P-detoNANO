#ifndef SCREENSTATES_H
#define SCREENSTATES_H

#include <Arduino.h>
#include <SSD1306/SSD1306Ascii.h>
#include <SSD1306/SSD1306AsciiAvrI2c.h>

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

/**
 * @brief Switches between screen states, refer to enum SCREEN_STATES
 * 
 * @param btnCenter: The pointer to the "debounced center button" object
 * @param btnRight: The pointer to the "debounced right button" object
 * @param btnLeft: The pointer to the "debounced left button" object
 * @param SSD1306AsciiAvrI2c: The SSD1306 object pointer
 * @param isUnlocked: The pointer to a global boolean variable which determines if the device is currently unlocked
 * @param screenIdx: The pointer to the index of the screen (refer to enum SCREEN_STATES)
 * @param config: The pointer of the current device's config
 * @param slaveConfig: The pointer of the slave's config, ONLY USED WHEN CALLING THIS FUNCTION FROM A MASTER, is nullptr by default
 */
void switchScreenState(SSD1306AsciiAvrI2c* oled, DebouncedButton* btnCenter, DebouncedButton* btnRight, DebouncedButton* btnLeft, uint8_t* isUnlocked, int *screenIdx, userconfig_s *config, userconfig_s *slaveConfig = nullptr);

#endif