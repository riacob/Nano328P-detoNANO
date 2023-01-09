# Project Objective
Selling a kit to create your very own transmitter/receiver pair of detoNANO, while mantaining everything open-source.
<br>
PCB and software are still in a development phase.

# Project Description
This project aims towards creating a 2.4GHz radio-controlled remote detonator.
<br>
It is composed of two distinct parts, the transmitter and the receiver.
<br>
The receiver, once turned on, waits for a device to connect, then allows the user to close its relay contact, which could be used to ignite a fuse through a nichrome wire.
<br>
Both devices are configurable trough an intuitive UI on an SSD1306 128*64 0.96" OLED display. The receiver can also remotely receive configuration data from the transmitter.
<br>
Setting a time delay before the detonation is possible.

# Safety Disclaimer
<br>
This device is intended exlusively for educational use. The makers do not condone its use for any violent acts towards persons, animals or objects.
<br>
The makers of this project assume no responsibility for any damage to persons, animals or objects derived from any use of the provided devices.
<br>
Once the data packet is sent, it takes a minuscule amount of time to reach the receiver. Never approach the receiver when a detonate command was sent.
<br>
A key is required to turn the devices on. Only users with a key will be able to use the devices.
<br>
Moreover, the priming switch (lever) is protected with a flip-up cover. The detonation button is actually composed of two series buttons, to be pressed one with each hand.
<br>
The abort button allows the user to send an abort command to the receiver, thus blocking both devices until the next restart.

# Required Components
This list excludes resistors

### Transmitter Only
- Nothing
- 2x Tactile push button

### Receiver Only
- 1x Relay
- 1x NPN transistor
- 1x Red LED

### Common
- 2x NRF24L01+PA SPI 2.4GHz transreceiver module (with antenna)
- 2x Arduino Nano ATmega328P
- 2x SSD1306 64x32 I2C OLED 0.96"
- 2x RGB LED
- 2x Key switch
- 4x Tactile push button
- 2x Buzzer (not self oscillating)
- 2x SPST lever switch w/ flip-up safety cover
- 2x USB-C power bank module
- 4x 18650 2200mAh li-ion battery
- 2x PCB front panel
- 2x 3D printed case

# Ideas and notes
- Tone library plays tones on the buzzer to give different audio feedbacks
- Programmable channels via OLED interface
- Programmable detonation delay (TX) via OLED interface
- Abort button to lock the unit until restarted (and eventually send emergency signal to the receiver)
- Power led blinks continuously on device lockup
- Enumeration containing shutdown/lockup states, where the latest shutdown state is saved in EEPROM and printed on the OLED at startup
- All the possible software and hardware safety routines

# Credits
Project by Riccardo Iacob & Fabio Grossi
<br>
Ideated on June 18, 2022
<br>
Started hardware development on December 29, 2022
<br>
Started firmware development on January 8, 2023
