#include "TrafficLight.h"

#define I_TURN_ON     1
#define I_TURN_OFF    2
#define I_SET_TIMER   3
#define I_SET_MODE    4

TrafficLight::TrafficLight() {
}

void TrafficLight::turnON(byte led) {
    Wire.beginTransmission(addr);
    Wire.write(I_TURN_ON);
    Wire.write(led);
    Wire.endTransmission();
}

void TrafficLight::turnOFF() {
    Wire.beginTransmission(addr);
    Wire.write(I_TURN_OFF);
    Wire.endTransmission();
}

void TrafficLight::setTimer(byte led, word time_ms) {
    Wire.beginTransmission(addr);
    Wire.write(I_SET_TIMER);
    Wire.write(led);
    Wire.write(highByte(time_ms));
    Wire.write(lowByte(time_ms));
    Wire.endTransmission();
}

void TrafficLight::setMode(byte runMode) {
    Wire.beginTransmission(addr);
    Wire.write(I_SET_MODE);
    Wire.write(runMode);
    Wire.endTransmission();
}
