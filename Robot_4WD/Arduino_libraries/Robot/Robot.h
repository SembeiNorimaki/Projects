#ifndef _Robot_h
#define _Robot_h

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>

class Robot {
public:
    Robot();
    void initialize(Adafruit_MotorShield);
    void set_speed(int, int);
    void test();

private:
    int left_speed;
    int right_speed;
    int ctrl_mode;
    Adafruit_DCMotor *motorFL;
    Adafruit_DCMotor *motorFR;
    Adafruit_DCMotor *motorRL;
    Adafruit_DCMotor *motorRR;

    void set_speed2(int, int);
};
#endif
