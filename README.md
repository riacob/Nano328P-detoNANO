# detoNANO
Radio controlled detonator (transmitter/receiver pair) based on Arduino Nano with ATmega328P.
<br>
Functional description
<br>
Two communicating devices* (transmitter/receiver or master/slave), once correctly configured, get linked together with a keepalive packet**.
<br>
After the user arms both devices, and steps away from the receiver to a safe distance, the user can arm the device and press both detonation buttons and wait, if set, for the timer to expire, and thus for the detonation to happen.
<br>
*An unbalanced mode with a master and multiple slaves is possible.
<br>
**The purpose of this packet is to ensure that the devices are always able to communicate. Lack of communication for a defined interval of time means that the devices will enter a safety-lock mode, in which they will need to be rebooted to work again.
<br>
Refer to the [Wiki](https://github.com/riacob/Nano328P-detoNANO/wiki) for more information.

# Hardware
![First Basic Kit Build](/docs/basic_model_kit_1.jpg "First Basic Kit Build")
First build of the "Basic" Kit
<br>
Note: the key switch has been bypassed by a jumper link, since this model will be powered exclusively from the Arduino's USB port.
<br>
The safety layer the key was meant to add is replaced by the ability to unplug the external power, which is impossible in the enclosed "Pro" and "Ultimate" models.
<br>
The devices, as stated in the documentation, can be PIN-protected either way, thus ensuring safety of operation.

# Software
Perhaps the most "boring" part of this project. As always, it's just lines of code, organized and commented in such a way that code is easy to read, debug, mantain and update, while also mantaining modularity. It might look a little bloated, but that's just how it is for big complex projects with modularity and ease of human-readability in mind.
<br>
All the coding was done using PlatformIO IDE in Visual Studio Code.

# Legal
This project is released under the BSD v2 License.
<br>
The following external libraries are used:
- [ArduinoJson (MIT)](https://github.com/bblanchon/ArduinoJson)
- [SSD1306Ascii (MIT)](https://github.com/greiman/SSD1306Ascii)
- [RF24 (GPL v2)](https://github.com/nRF24/RF24)
<br>

Disclaimers:
- This device is intented for educational use only.
- The makers of this project are not responsible for any damage caused to objects, animals or persons.
- This device is not intended for laymen.
- This documentation could be changed at any time without previous announcement, thus it's the user's duty to check for updates.
