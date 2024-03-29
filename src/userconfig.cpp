/**
 * @file userconfig.cpp
 * @author Riccardo Iacob
 * @brief Implementation of "userconfig.h"
 * @version 0.1
 * @date 2023-01-08
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "userconfig.h"
#include "hardware.h"
#include "debug.h"

void setDefaultMasterConfig(userconfig_s *config)
{
    config->radioChannel = 76;

    config->targetID[0] = 'S';
    config->targetID[1] = config->radioChannel;
    config->targetID[2] = 0;
    config->targetID[3] = 0;
    config->targetID[4] = 0;

    config->ownID[0] = 'M';
    config->ownID[1] = config->radioChannel;
    config->ownID[2] = 0;
    config->ownID[3] = 0;
    config->ownID[4] = 0;

    config->detonationDelay = 0;

    config->pinEnabled = 1;

    config->pin[0] = 0;
    config->pin[1] = 0;
    config->pin[2] = 0;
    config->pin[3] = 0;

    config->detonationPulseTime = 500;
}

void setDefaultSlaveConfig(userconfig_s *config)
{
    config->radioChannel = 76;

    config->targetID[0] = 'M';
    config->targetID[1] = config->radioChannel;
    config->targetID[2] = 0;
    config->targetID[3] = 0;
    config->targetID[4] = 0;

    config->ownID[0] = 'S';
    config->ownID[1] = config->radioChannel;
    config->ownID[2] = 0;
    config->ownID[3] = 0;
    config->ownID[4] = 0;

    config->detonationDelay = 0;

    config->pinEnabled = 1;

    config->pin[0] = 0;
    config->pin[1] = 0;
    config->pin[2] = 0;
    config->pin[3] = 0;

    config->detonationPulseTime = 500;
}

void transmitConfigToSlave(userconfig_s *config, RF24 *radio)
{
    // ? is inserting a delay between each packet beneficial?
    // Convert config to a series of packets to be sent to the slave, containing data in JSON format
    // Each with the same command, but different contents
    // CMD_BYTE{json}

    // JsonDocument
    DynamicJsonDocument json(64);
    // Buffer that will contain the serialized JSON and that will be sent to the slave
    uint8_t buf[32] = {0};
    // Iterator variable
    int i;
    // Acknowledge bit
    bool ack = false;
    // Number of times the master tried unsuccessfully to send the config to the slave
    // Declared as static because it should remain the same troughout all the times the function is recursively called
    // And be automatically reset to zero at the end of the function (if there are no issues)
    static int tries = 0;
    const int maxtries = 5;

    // radioChannel
    json["rc"] = config->radioChannel;
    serializeJson(json, buf);
    json.clear();
    // Shift buf[] by 1
    for (i = 31; i > 0; i--)
    {
        buf[i] = buf[i - 1];
    }
    // Insert cmd at buf[0]
    buf[0] = CMD_CONFIG;
    // Try to write to radio
    ack = radio->write(buf, 32);
    if (!ack && tries < maxtries)
    {
        transmitConfigToSlave(config, radio);
        tries++;
    }

    // targetID
    json["tid"][0] = config->targetID[0];
    json["tid"][1] = config->targetID[1];
    json["tid"][2] = config->targetID[2];
    json["tid"][3] = config->targetID[3];
    json["tid"][4] = config->targetID[4];
    serializeJson(json, buf);
    json.clear();
    // Shift buf[] by 1
    for (i = 31; i > 0; i--)
    {
        buf[i] = buf[i - 1];
    }
    // Insert cmd at buf[0]
    buf[0] = CMD_CONFIG;
    // Try to write to radio
    ack = radio->write(buf, 32);
    if (!ack && tries < maxtries)
    {
        transmitConfigToSlave(config, radio);
        tries++;
    }

    // ownID
    json["oid"][0] = config->ownID[0];
    json["oid"][1] = config->ownID[1];
    json["oid"][2] = config->ownID[2];
    json["oid"][3] = config->ownID[3];
    json["oid"][4] = config->ownID[4];
    serializeJson(json, buf);
    json.clear();
    // Shift buf[] by 1
    for (i = 31; i > 0; i--)
    {
        buf[i] = buf[i - 1];
    }
    // Insert cmd at buf[0]
    buf[0] = CMD_CONFIG;
    // Try to write to radio
    ack = radio->write(buf, 32);
    if (!ack && tries < maxtries)
    {
        transmitConfigToSlave(config, radio);
        tries++;
    }

    // detonationDelay
    json["dd"] = config->detonationDelay;
    serializeJson(json, buf);
    json.clear();
    // Shift buf[] by 1
    for (i = 31; i > 0; i--)
    {
        buf[i] = buf[i - 1];
    }
    // Insert cmd at buf[0]
    buf[0] = CMD_CONFIG;
    // Try to write to radio
    ack = radio->write(buf, 32);
    if (!ack && tries < maxtries)
    {
        transmitConfigToSlave(config, radio);
        tries++;
    }

    // pinEnabled
    json["pe"] = config->pinEnabled;
    serializeJson(json, buf);
    json.clear();
    // Shift buf[] by 1
    for (i = 31; i > 0; i--)
    {
        buf[i] = buf[i - 1];
    }
    // Insert cmd at buf[0]
    buf[0] = CMD_CONFIG;
    // Try to write to radio
    ack = radio->write(buf, 32);
    if (!ack && tries < maxtries)
    {
        transmitConfigToSlave(config, radio);
        tries++;
    }

    // pin
    json["pin"][0] = config->pin[0];
    json["pin"][1] = config->pin[1];
    json["pin"][2] = config->pin[2];
    json["pin"][3] = config->pin[3];
    serializeJson(json, buf);
    json.clear();
    // Shift buf[] by 1
    for (i = 31; i > 0; i--)
    {
        buf[i] = buf[i - 1];
    }
    // Insert cmd at buf[0]
    buf[0] = CMD_CONFIG;
    // Try to write to radio
    ack = radio->write(buf, 32);
    if (!ack && tries < maxtries)
    {
        transmitConfigToSlave(config, radio);
        tries++;
    }

    // detonationPulseTime
    json["dpt"] = config->detonationPulseTime;
    serializeJson(json, buf);
    json.clear();
    // Shift buf[] by 1
    for (i = 31; i > 0; i--)
    {
        buf[i] = buf[i - 1];
    }
    // Insert cmd at buf[0]
    buf[0] = CMD_CONFIG;
    // Try to write to radio
    ack = radio->write(buf, 32);
    if (!ack && tries < maxtries)
    {
        transmitConfigToSlave(config, radio);
        tries++;
    }

    if (tries >= maxtries)
    {
        debugln("config not written");
        tries = 0;
    }
    else
    {
        debugln("successfully written config");
        tries = 0;
    }
}

int receiveConfigFromMaster(userconfig_s *config, uint8_t *dataBuffer)
{
    // Decode the incoming data from the slave
    // Data will start with a byte that is a command (used in the command interpreter)
    // Data after the closing data could be anything
    // For those two reasons, we need to produce a clean buffer to be deserialized into a JSON

    // JsonDocument
    DynamicJsonDocument json(64);
    // Start of JSON { (no nested objects)
    int indexOfBracket1 = -1;
    // End of JSON } (no nested objects)
    int indexOfBracket2 = -1;
    // Length of json {json}
    int lengthOfJson = 0;
    // Iterator variable
    int i;

    // Trim data buffer so it contains json only
    // CMD_BYTE{json}???????? -> {json}
    for (i = 0; i < 32; i++)
    {
        // Find irst index of {
        if ((dataBuffer[i] == '{') && (indexOfBracket1 <= -1))
        {
            indexOfBracket1 = i;
        }
        // Find first index of }
        if ((dataBuffer[i] == '}') && (indexOfBracket2 <= -1))
        {
            indexOfBracket2 = i;
        }
    }
    // Get length of JSON
    lengthOfJson = indexOfBracket2 - indexOfBracket1 + 1;
    // Create a new buffer that exclusively contains the JSON
    uint8_t cleanBuffer[lengthOfJson] = {0};
    for (i = 0; i < lengthOfJson; i++)
    {
        cleanBuffer[i] = dataBuffer[i + indexOfBracket1];
    }

    deserializeJson(json, cleanBuffer);

    // Automatically find the key-value pair in the json (only one per packet)
    // radioChannel
    if (json.containsKey("rc"))
    {
        config->radioChannel = json["rc"];
        return -1;
    }
    // targetID
    else if (json.containsKey("tid"))
    {
        config->targetID[0] = json["tid"][0];
        config->targetID[1] = json["tid"][1];
        config->targetID[2] = json["tid"][2];
        config->targetID[3] = json["tid"][3];
        config->targetID[4] = json["tid"][4];
        return -1;
    }
    // ownID
    else if (json.containsKey("oid"))
    {
        config->ownID[0] = json["oid"][0];
        config->ownID[1] = json["oid"][1];
        config->ownID[2] = json["oid"][2];
        config->ownID[3] = json["oid"][3];
        config->ownID[4] = json["oid"][4];
        return -1;
    }
    // detonationDelay
    else if (json.containsKey("dd"))
    {
        config->detonationDelay = json["dd"];
        return -1;
    }
    // pinEnabled
    else if (json.containsKey("pe"))
    {
        config->pinEnabled = json["pe"];
        return -1;
    }
    // pin
    else if (json.containsKey("pin"))
    {
        config->pin[0] = json["pin"][0];
        config->pin[1] = json["pin"][1];
        config->pin[2] = json["pin"][2];
        config->pin[3] = json["pin"][3];
        return -1;
    }
    // detonationPulseTime
    else if (json.containsKey("dpt"))
    {
        config->detonationPulseTime = json["dpt"];
        return -1;
    }
    // If there are no valid keys, we might have a parsing error
    else
    {
        return 1;
    }
}

void printConfig(userconfig_s *config)
{
    int i;
    debug("Role: ");
    if (config->ownID[0] == 'M')
    {
        debugln("TRANSMITTER");
    }
    else
    {
        debugln("RECEIVER");
    }
    debug("radioChannel: ");
    debugln(config->radioChannel);
    debug("targetID (char): ");
    for (i = 0; i < 5; i++)
    {
        debug((char)config->targetID[i]);
        debug(" ");
    }
    debugln();
    debug("targetID (int): ");
    for (i = 0; i < 5; i++)
    {
        debug((int)config->targetID[i]);
        debug(" ");
    }
    debugln();
    debug("ownID (char): ");
    for (i = 0; i < 5; i++)
    {
        debug((char)config->ownID[i]);
        debug(" ");
    }
    debugln();
    debug("ownID (int): ");
    for (i = 0; i < 5; i++)
    {
        debug((int)config->targetID[i]);
        debug(" ");
    }
    debugln();
    debug("detonationDelay: ");
    debugln(config->detonationDelay);
    debug("pinEnabled: ");
    debugln(config->pinEnabled ? "yes" : "no");
    debug("pin: ");
    for (i = 0; i < 4; i++)
    {
        debug((int)config->pin[i]);
        debug(" ");
    }
    debugln();
    debug("detonationPulseTime: ");
    debug(config->detonationPulseTime);
    debugln();
}

void writeConfig(userconfig_s *config)
{
    // Write the struct to the EEPROM
    EEPROM.put(0, *config);
}

void readConfig(userconfig_s *config)
{
    // Read the struct from the EEPROM
    EEPROM.get(0, *config);
}

void printEEPROM()
{
    // Print EEPROM contents, with a newline every 32 bytes
    uint8_t j = 0;
    for (uint16_t i = 0; i < EEPROM.length(); i++)
    {
        debug((int)EEPROM.read(i));
        debug(" ");
        j++;
        if (j == 32)
        {
            debugln();
            j = 0;
        }
    }
}
