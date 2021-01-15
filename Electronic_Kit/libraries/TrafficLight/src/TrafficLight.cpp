#include "TrafficLight.h"

TrafficLight::TrafficLight() {

}

void TrafficLight::initialize() {
    instructions = 
}

void TrafficLight::sendData() {

}

void TrafficLight::setMode(byte mode_) {
    instructions.push_back(I_SET_MODE);
    instructions.push_back(mode_);
}

void TrafficLight::turnON(byte led) {
    instructions.push_back(I_TURN_ON);
    instructions.push_back(led);
}

void TrafficLight::setTimer(byte led, word time_ms) {
    instructions.push_back(I_SET_TIMER);
    instructions.push_back(led);
    instructions.push_back(time_ms);
}
