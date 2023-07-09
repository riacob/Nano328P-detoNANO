#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

#define DEBUG true

#if DEBUG == false
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

#endif