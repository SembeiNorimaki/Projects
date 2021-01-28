/* Author: Isaac Alonso
*
*  Example of TrafficLight in manual mode
*  In manual mode the traffic light will be operated by giving commands.
*
*  List of commands:
*    TrafficLight::turnOn(byte led);
*    TrafficLight::turnOff();
*
*  List of constants:
*    TrafficLight::LED_GREEN
*    TrafficLight::LED_YELLOW
*    TrafficLight::LED_RED
*/

#include <TrafficLight.h>

tf = TrafficLight();

void setup() {
    tf.setMode(tf.MODE_MANUAL);
}

void loop() {
    tf.turnOn(tf.LED_GREEN);
    delay(4000);
    tf.turnOn(tf.LED_YELLOW);
    delay(2000);
    tf.turnOn(tf.LED_RED);
    delay(5000);
}