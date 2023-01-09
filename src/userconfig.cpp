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
}

void writeConfig(userconfig_s *config)
{
    config->targetID[1] = config->radioChannel;
    config->ownID[1] = config->radioChannel;

    EEPROM.put(0, *config);
}

void readConfig(userconfig_s *config)
{
    EEPROM.get(0, *config);
}

void printConfig(userconfig_s *config)
{
    Serial.print("radioChannel: ");
    Serial.println(config->radioChannel);
    Serial.print("targetID (char): ");
    for (int i = 0; i < 5; i++)
    {
        Serial.print((char)config->targetID[i]);
        Serial.print(" ");
    }
    Serial.println();
    Serial.print("targetID (int): ");
    for (int i = 0; i < 5; i++)
    {
        Serial.print((int)config->targetID[i]);
        Serial.print(" ");
    }
    Serial.println();
    Serial.print("ownID (char): ");
    for (int i = 0; i < 5; i++)
    {
        Serial.print((char)config->ownID[i]);
        Serial.print(" ");
    }
    Serial.println();
    Serial.print("ownID (int): ");
    for (int i = 0; i < 5; i++)
    {
        Serial.print((int)config->targetID[i]);
        Serial.print(" ");
    }
    Serial.println();
    Serial.print("detonationDelay: ");
    Serial.println(config->detonationDelay);
}

void transmitConfigToSlave(userconfig_s *config, RF24 *radio)
{
    DynamicJsonDocument json(64);
    uint8_t buf[32];

    json["rc"] = config->radioChannel;
    serializeJson(json, buf);
    serializeJson(json, Serial);
    json.clear();
    radio->write(buf, 32);

    json["tid"][0] = config->targetID[0];
    json["tid"][1] = config->targetID[1];
    json["tid"][2] = config->targetID[2];
    json["tid"][3] = config->targetID[3];
    json["tid"][4] = config->targetID[4];
    serializeJson(json, buf);
    serializeJson(json, Serial);
    json.clear();
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
    radio->write(buf, 32);

    json["dd"] = config->detonationDelay;
    serializeJson(json, buf);
    serializeJson(json, Serial);
    Serial.println();
    json.clear();
    radio->write(buf, 32);
}

int receiveConfigFromMaster(userconfig_s *config, uint8_t *dataBuffer)
{
    DynamicJsonDocument json(64);
    deserializeJson(json, dataBuffer);

    // Automatically find the key-value pair in the json (only one per packet)
    if (json.containsKey("rc"))
    {
        config->radioChannel = json["rc"];
        return -1;
    }
    else if (json.containsKey("tid"))
    {
        config->targetID[0] = json["tid"][0];
        config->targetID[1] = json["tid"][1];
        config->targetID[2] = json["tid"][2];
        config->targetID[3] = json["tid"][3];
        config->targetID[4] = json["tid"][4];
        return -1;
    }
    else if (json.containsKey("oid"))
    {
        config->ownID[0] = json["oid"][0];
        config->ownID[1] = json["oid"][1];
        config->ownID[2] = json["oid"][2];
        config->ownID[3] = json["oid"][3];
        config->ownID[4] = json["oid"][4];
        return -1;
    }
    else if (json.containsKey("dd"))
    {
        config->detonationDelay = json["dd"];
    }
    // If there are no valid keys, we might have a parsing error
    else
    {
        return 1;
    }
}

void printEEPROM()
{
    uint8_t j = 0;
    for (uint16_t i = 0; i < EEPROM.length(); i++)
    {
        Serial.print((int)EEPROM.read(i));
        Serial.print(" ");
        j++;
        if (j == 32) {
            Serial.println();
            j = 0;
        }
    }
}