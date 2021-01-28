/*
* Example with modules: Traffic Light, Color Sensor, Encoder, Touch Sensors
*
*/

// #include <TrafficLight.h>
#include <Wire.h>

// tf = TrafficLight();
// di = DigitalInput();

void setup () {
    Serial.begin(115200);
    Serial.println("Hello");
    delay(1000);
    Wire.beginTransmission(11); 
    // Wire.write(byte(3));  // set timer
    
    // Wire.write(byte(0x08));
    // Wire.write(byte(0x00));  // red time 2028

    // Wire.write(byte(0x04));
    // Wire.write(byte(0x00));   // yellow time 1024
    
    // Wire.write(byte(0x10));
    // Wire.write(byte(0x00));    // green time 4096
    Wire.write(byte(2));
    Wire.endTransmission();
    // Wire.requestFrom(11, 1);
    // byte rx = Wire.read();
    // Serial.println(rx);
    delay(1000);
    Serial.println("Done");
}

void loop() {

}