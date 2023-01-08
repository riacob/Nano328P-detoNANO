// 8 jan 2023
#include "userconfig.h"

void setDefaultMasterConfig(userconfig_s *config)
{
    config->radioChannel = 76;

    config->targetAddress[0] = 'S';
    config->targetAddress[1] = config->radioChannel;
    config->targetAddress[2] = 0;
    config->targetAddress[3] = 0;
    config->targetAddress[4] = 0;

    config->ownAddress[0] = 'M';
    config->ownAddress[1] = config->radioChannel;
    config->ownAddress[2] = 0;
    config->ownAddress[3] = 0;
    config->ownAddress[4] = 0;

    config->detonationDelay = 0;
}

void setDefaultSlaveConfig(userconfig_s *config)
{
    config->radioChannel = 76;

    config->targetAddress[0] = 'M';
    config->targetAddress[1] = config->radioChannel;
    config->targetAddress[2] = 0;
    config->targetAddress[3] = 0;
    config->targetAddress[4] = 0;

    config->ownAddress[0] = 'S';
    config->ownAddress[1] = config->radioChannel;
    config->ownAddress[2] = 0;
    config->ownAddress[3] = 0;
    config->ownAddress[4] = 0;

    config->detonationDelay = 0;
}

void writeConfig(userconfig_s *config)
{
    config->targetAddress[1] = config->radioChannel;
    config->ownAddress[1] = config->radioChannel;

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
    Serial.print("targetAddress (char): ");
    for (int i = 0; i < 5; i++)
    {
        Serial.print((char)config->targetAddress[i]);
        Serial.print(" ");
    }
    Serial.println();
    Serial.print("targetAddress (int): ");
    for (int i = 0; i < 5; i++)
    {
        Serial.print((int)config->targetAddress[i]);
        Serial.print(" ");
    }
    Serial.println();
    Serial.print("ownAddress (char): ");
    for (int i = 0; i < 5; i++)
    {
        Serial.print((char)config->ownAddress[i]);
        Serial.print(" ");
    }
    Serial.println();
    Serial.print("ownAddress (int): ");
    for (int i = 0; i < 5; i++)
    {
        Serial.print((int)config->targetAddress[i]);
        Serial.print(" ");
    }
    Serial.println();
    Serial.print("detonationDelay: ");
    Serial.println(config->detonationDelay);
}

void transmitConfigToSlave(userconfig_s *config, RF24 *radio)
{
    DynamicJsonDocument json(32);
    uint8_t buf[32];

    json["rc"] = config->radioChannel;
    serializeJson(json, buf);
    serializeJson(json, Serial);
    json.clear();
    radio->write(buf, 32);

    json["ta"][0] = config->targetAddress[0];
    json["ta"][1] = config->targetAddress[1];
    json["ta"][2] = config->targetAddress[2];
    json["ta"][3] = config->targetAddress[3];
    json["ta"][4] = config->targetAddress[4];
    serializeJson(json, buf);
    serializeJson(json, Serial);
    json.clear();
    radio->write(buf, 32);

    json["oa"][0] = config->ownAddress[0];
    json["oa"][1] = config->ownAddress[1];
    json["oa"][2] = config->ownAddress[2];
    json["oa"][3] = config->ownAddress[3];
    json["oa"][4] = config->ownAddress[4];
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
    DynamicJsonDocument json(32);
    deserializeJson(json, dataBuffer);

    // Automatically find the key-value pair in the json (only one per packet)
    if (json.containsKey("rc"))
    {
        config->radioChannel = json["rc"];
        return -1;
    }
    else if (json.containsKey("ta"))
    {
        config->targetAddress[0] = json["ta"][0];
        config->targetAddress[1] = json["ta"][1];
        config->targetAddress[2] = json["ta"][2];
        config->targetAddress[3] = json["ta"][3];
        config->targetAddress[4] = json["ta"][4];
        return -1;
    }
    else if (json.containsKey("oa"))
    {
        config->ownAddress[0] = json["oa"][0];
        config->ownAddress[1] = json["oa"][1];
        config->ownAddress[2] = json["oa"][2];
        config->ownAddress[3] = json["oa"][3];
        config->ownAddress[4] = json["oa"][4];
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