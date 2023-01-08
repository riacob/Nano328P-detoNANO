// 8 jan 2023
#ifndef USERCONFIG_H
#define USERCONFIG_H

#include <Arduino.h>
#include <EEPROM.h>
#include <RF24.h>
#include <ArduinoJSON.h>

typedef struct
{
    // JSON key: rc
    uint8_t radioChannel;
    // JSON key: ta
    uint8_t targetAddress[5];
    // JSON key: oa
    uint8_t ownAddress[5];
    // JSON key: dd
    uint32_t detonationDelay;
} userconfig_s;

/**
 * @brief Prints config to serial port
 *
 * @param config: The target config
 */
void printConfig(userconfig_s *config);
/**
 * @brief Sets the default master config
 *
 * @param config: The target config
 */
void setDefaultMasterConfig(userconfig_s *config);
/**
 * @brief Sets the default slave config
 *
 * @param config: The target config
 */
void setDefaultSlaveConfig(userconfig_s *config);
/**
 * @brief Writes config to EEPROM
 *
 * @param config: The target config
 */
void writeConfig(userconfig_s *config);
/**
 * @brief Reads config from EEPROM
 *
 * @param config: The target config
 */
void readConfig(userconfig_s *config);
/**
 * @brief Transmits config from master to slave
 *
 * @param config: The target config
 */
void transmitConfigToSlave(userconfig_s *config, RF24 *radio);
/**
 * @brief Handles the reception of the config from master
 * After the "config incoming command", the folliwing 32 bytes are one of the config's key-value pairs, in JSON format (indexes 1-31 in dataBuffer)
 * This function reads such kvp and saves them to the config
 * 
 * @note This function does NOT write config to EEPROM
 * 
 * @param config: The target config
 * @return int: Operation status code
 */
int receiveConfigFromMaster(userconfig_s *config, uint8_t *dataBuffer);

/**
 * @brief Prints the full EEPROM contents to serial
 * 
 */
void printEEPROM();

#endif