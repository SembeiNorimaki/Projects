#include "TrafficLight.h"

#define I_TURN_ON     1
#define I_TURN_OFF    2
#define I_SET_TIMER   3
#define I_SET_MODE    4

TrafficLight::TrafficLight() {
}

void TrafficLight::initialize() {
}

void TrafficLight::turnON(byte led) {
    Wire.write(I_TURN_ON);
    Wire.write(led);
}

void TrafficLight::turnOFF() {
    Wire.write(I_TURN_OFF);
}

void TrafficLight::setTimer(byte led, word time_ms) {
    Wire.write(I_SET_TIMER);
    Wire.write(led);
    Wire.write(highByte(time_ms));
    Wire.write(lowByte(time_ms));
}

void TrafficLight::setMode(byte runMode) {
    Wire.write(I_SET_MODE);
    Wire.write(runMode);
}
