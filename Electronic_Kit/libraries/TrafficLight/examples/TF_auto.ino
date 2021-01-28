/* 
*   Example of TrafficLight in automatic mode
*   Author: Isaac Alonso
*/

#include <TrafficLight.h>

tf = TrafficLight();

void setup() {
    tf.setTimer(tf.LED_GREEN, 3000);
    tf.setTimer(tf.LED_YELLOW, 1000);
    tf.setTimer(tf.LED_RED, 5000);
    tf.setMode(tf.MODE_AUTO);
}

void loop() {
}
