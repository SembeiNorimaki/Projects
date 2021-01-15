#ifndef _TrafficLight_h
#define _TrafficLight_h

#include <Arduino.h>
#include <Wire.h>
#include <StandardCplusplus.h>
#include <vector>

class TrafficLight {
public:
    TrafficLight();
    const int LED_GREEN = 1;
    const int LED_YELLOW = 2;
    const int LED_RED = 3;
    
    void setMode(byte mode_);
    void turnON(byte led);
    void turnOFF(byte led);
    void setTimer(byte led, word time_ms);

private:
    std::vector<int> instructions;
};
#endif