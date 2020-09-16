#include "Robot.h"

#define MODE_RC 0
#define MODE_CMD 1

Robot::Robot() {
}

void Robot::initialize(Adafruit_MotorShield AFMS) {
    motorFR = AFMS.getMotor(1);
    motorFL = AFMS.getMotor(2);
    motorRL = AFMS.getMotor(3);
    motorRR = AFMS.getMotor(4);

    //deadReckoner = dr; 
    int ctrl_mode = MODE_RC;

    int right_speed = 0;
    int left_speed = 0;
}

// void set_mode(int new_mode) {
//     ctrl_mode = new_mode; 
// }

void Robot::set_speed2(int left_speed, int right_speed) {
    /*
    Set the speed for left and right motor.
    Range: -255..255
    Negative values make the motor turn backwards
    */
    // Serial.print("Setting speed to ");
    // Serial.print(left_speed);
    // Serial.print(", ");
    // Serial.println(right_speed);

    if (left_speed > 0) {
        motorFL->run(FORWARD);
        motorRL->run(FORWARD);
        // deadReckoner.setLeftOmegaDirection = 1;
    } else if (left_speed < 0){
        motorFL->run(BACKWARD);
        motorRL->run(BACKWARD);
        // deadReckoner.setLeftOmegaDirection = -1;
    } else {
        motorFL->run(RELEASE);
        motorRL->run(RELEASE);
    }

    if (right_speed > 0) {
        motorFR->run(FORWARD);
        motorRR->run(FORWARD);
        // deadReckoner.setRightOmegaDirection = 1;
    } else if (right_speed < 0){
        motorFR->run(BACKWARD);
        motorRR->run(BACKWARD);
        // deadReckoner.setRightOmegaDirection = -1;
    } else {
        motorFR->run(RELEASE);
        motorRR->run(RELEASE);
    }

    motorFL->setSpeed(abs(left_speed));
    motorFR->setSpeed(abs(right_speed));
    motorRL->setSpeed(abs(left_speed));
    motorRR->setSpeed(abs(right_speed));
}

void Robot::set_speed(int target_left_speed, int target_right_speed) {
    // we need to accelerate to reach the target speed
    // stepzize = (stop - start)./(num_elements-1)
    int numel = max(abs(target_left_speed - left_speed), abs(target_right_speed - right_speed));
    double left_step = (target_left_speed - left_speed) / (numel - 1);
    double right_step = (target_right_speed - right_speed) / (numel - 1);
    
    for(int i=0; i<numel; i++) {
        set_speed2(int(left_speed + i*left_step), int(right_speed + i*right_step));
        delay(5);
    }
    set_speed2(target_left_speed, target_right_speed);

    left_speed = target_left_speed;
    right_speed = target_right_speed;
}

// void process_serial(serial_data) {
//     if (mode == MODE_RC) {
        
//     } else {

//     }
// }
