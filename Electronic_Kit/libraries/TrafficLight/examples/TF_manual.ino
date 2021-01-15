// Author: Isaac Alonso
//
// Example of TrafficLight in manual mode
// In manual mode the traffic light will be operated by giving commands.
//
// Available commands:
//   - TrafficLight::turn_on(byte led, bool blink);

//   led can be LED_GREEN, LED_YELLOW or LED_RED
//   blink makes the led blink (default false)


#include <TrafficLight.h>

tf = TrafficLight();

void setup() {
    tf.setMode(tf.MODE_MANUAL);
}

void loop() {
    tf.turn_on(tf.LED_GREEN);
    delay(4000);
    tf.turn_on(tf.LED_YELLOW, true);
    delay(2000);
    tf.turn_on(tf.LED_RED);
    delay(5000);
}