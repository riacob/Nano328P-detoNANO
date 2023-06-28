/**
 * @file hardware.h
 * @author Riccardo Iacob
 * @brief 
 * @version 0.1
 * @date 2023-01-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <RF24.h>

// Common GPIOs
#define PIN_LED_R A2
#define PIN_LED_G A1
#define PIN_LED_B A0
#define PIN_BUZZER A3
#define PIN_BTN_ABORT 3
#define PIN_BTN_CENTER 6
#define PIN_BTN_RIGHT 5
#define PIN_BTN_LEFT 4
#define PIN_RF24_IRQ 2
#define PIN_RF24_CE 9
#define PIN_RF24_CSN 10
#define PIN_ARM 7
// Transmitter GPIOs
#define PIN_DETONATE 8
// Receiver GPIOs
#define PIN_RELAY 8
#define PIN_SIREN A6

#define I2C_OLED_ADDRESS 0x3C
#define BAUDRATE 115200
#define RF24_PA_LEVEL RF24_PA_HIGH
#define RF24_DATARATE RF24_250KBPS
#define RF24_RETRIES_COUNT 15
#define RF24_RETRIES_DELAY 15
#define DEBUG true
#define WATCHDOG_TIMEOUT_MILLIS 10000

// Commands in packets
#define CMD_DETONATE 'D'
#define CMD_CONFIG 'C'
#define CMD_ABORT 'A'
#define CMD_WATCHDOG 'W'

#endif