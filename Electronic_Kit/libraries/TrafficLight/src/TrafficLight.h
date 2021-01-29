#ifndef _TrafficLight_h
#define _TrafficLight_h

#include <Arduino.h>
#include <Wire.h>

class TrafficLight {
public:
    TrafficLight();
    const byte LED_GREEN = 1;
    const byte LED_YELLOW = 2;
    const byte LED_RED = 3;
    const byte MODE_AUTO = 1;
    const byte MODE_MANUAL = 2;
    
    void turnON(byte led);
    void turnOFF();
    void setTimer(byte led, word time_ms);
    void setMode(byte runMode);

private:
    byte addr = 2;
};
#endif
