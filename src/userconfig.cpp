/**
 * @file userconfig.cpp
 * @author Riccardo Iacob
 * @brief
 * @version 0.1
 * @date 2023-01-08
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "userconfig.h"

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

    config->pinEnabled = 0;

    config->pin[0] = 0;
    config->pin[1] = 0;
    config->pin[2] = 0;
    config->pin[3] = 0;
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

    config->pinEnabled = 0;

    config->pin[0] = 0;
    config->pin[1] = 0;
    config->pin[2] = 0;
    config->pin[3] = 0;
}

void printConfig(userconfig_s *config)
{
    int i;
    Serial.print("radioChannel: ");
    Serial.println(config->radioChannel);
    Serial.print("targetID (char): ");
    for (i = 0; i < 5; i++)
    {
        Serial.print((char)config->targetID[i]);
        Serial.print(" ");
    }
    Serial.println();
    Serial.print("targetID (int): ");
    for (i = 0; i < 5; i++)
    {
        Serial.print((int)config->targetID[i]);
        Serial.print(" ");
    }
    Serial.println();
    Serial.print("ownID (char): ");
    for (i = 0; i < 5; i++)
    {
        Serial.print((char)config->ownID[i]);
        Serial.print(" ");
    }
    Serial.println();
    Serial.print("ownID (int): ");
    for (i = 0; i < 5; i++)
    {
        Serial.print((int)config->targetID[i]);
        Serial.print(" ");
    }
    Serial.println();
    Serial.print("detonationDelay: ");
    Serial.println(config->detonationDelay);
    Serial.print("pinEnabled: ");
    Serial.println(config->pinEnabled);
    Serial.print("pin: ");
    for (i = 0; i < 4; i++)
    {
        Serial.print((int)config->pin[i]);
    }
    Serial.println();
}

// TODO TEST SHIFT-INSERT
void transmitConfigToSlave(userconfig_s *config, RF24 *radio)
{
    DynamicJsonDocument json(64);
    uint8_t buf[32];
    int i;

    json["rc"] = config->radioChannel;
    serializeJson(json, buf);
    serializeJson(json, Serial);
    json.clear();
    // Shift buf[] by 1
    for (i = 31; i > 0; i--)
    {
        buf[i] = buf[i - 1];
    }
    // Insert cmd at buf[0]
    buf[0] = 'C';
    for (int j = 0; j < 32; j++)
    {
        Serial.print((char)buf[j]);
    }
    radio->write(buf, 32);

    json["tid"][0] = config->targetID[0];
    json["tid"][1] = config->targetID[1];
    json["tid"][2] = config->targetID[2];
    json["tid"][3] = config->targetID[3];
    json["tid"][4] = config->targetID[4];
    serializeJson(json, buf);
    serializeJson(json, Serial);
    json.clear();
    // Shift buf[] by 1
    // Insert cmd at buf[0]
    radio->write(buf, 32);

    json["oid"][0] = config->ownID[0];
    json["oid"][1] = config->ownID[1];
    json["oid"][2] = config->ownID[2];
    json["oid"][3] = config->ownID[3];
    json["oid"][4] = config->ownID[4];
    serializeJson(json, buf);
    serializeJson(json, Serial);
    Serial.println();
    json.clear();
    // Shift buf[] by 1
    // Insert cmd at buf[0]
    radio->write(buf, 32);

    json["dd"] = config->detonationDelay;
    serializeJson(json, buf);
    serializeJson(json, Serial);
    Serial.println();
    json.clear();
    // Shift buf[] by 1
    // Insert cmd at buf[0]
    radio->write(buf, 32);

    json["pin"][0] = config->ownID[0];
    json["pin"][1] = config->ownID[1];
    json["pin"][2] = config->ownID[2];
    json["pin"][3] = config->ownID[3];
    serializeJson(json, buf);
    serializeJson(json, Serial);
    Serial.println();
    json.clear();
    // Shift buf[] by 1
    // Insert cmd at buf[0]
    radio->write(buf, 32);
}

// TODO check if this works with unclean array, i.e riuhh{json}hahfh
int receiveConfigFromMaster(userconfig_s *config, uint8_t *dataBuffer)
{
    DynamicJsonDocument json(64);
    deserializeJson(json, dataBuffer);

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
    // If there are no valid keys, we might have a parsing error
    else
    {
        return 1;
    }
}

void writeConfig(userconfig_s *config)
{
    // The first byte of the ID will always be the device role, S (slave) or M (master)
    // The second byte of the ID will always be the radio channel
    config->targetID[1] = config->radioChannel;
    config->ownID[1] = config->radioChannel;
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
        Serial.print((int)EEPROM.read(i));
        Serial.print(" ");
        j++;
        if (j == 32)
        {
            Serial.println();
            j = 0;
        }
    }
}