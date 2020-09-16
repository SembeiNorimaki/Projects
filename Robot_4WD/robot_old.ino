#include <Wire.h>
#include <Adafruit_MotorShield.h>
//#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <DeadReckoner.h>

// encoders
#define p_enc_l 2
#define p_enc_r 3

#define RADIUS 16.0         // wheel radius in mm
#define LENGTH 150.0        // wheel base length in mm
#define TICKS_PER_REV 900.0

// TIME INTERVALS
#define POSITION_COMPUTE_INTERVAL 50     // milliseconds
//#define SEND_INTERVAL 1000             // milliseconds

// ultrasonic sensor
#define p_trig 9
#define p_echo 10

// instructions
#define I_NONE 0
#define I_SET_SPEED_FR 'A'
#define I_SET_SPEED_FL 'B'
#define I_SET_SPEED_RR 'C'
#define I_SET_SPEED_RL 'D'
#define I_STOP 'E'
#define I_MOV_DIST 'F'
#define I_TURN_RIGHT 'G'
#define I_TURN_LEFT 'H'

// states
#define S_NONE 0
#define S_NEXT_INSTR 1
#define S_WAIT_DISTANCE 2
#define S_WAIT_ANGLE 3
#define S_OBSTACLE 4

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *motorFR = AFMS.getMotor(1);
Adafruit_DCMotor *motorFL = AFMS.getMotor(2);
Adafruit_DCMotor *motorRL = AFMS.getMotor(3);
Adafruit_DCMotor *motorRR = AFMS.getMotor(4);

volatile unsigned long leftTicks, rightTicks;
DeadReckoner deadReckoner(&leftTicks, &rightTicks, POSITION_COMPUTE_INTERVAL, TICKS_PER_REV, RADIUS, LENGTH);

int state = S_NONE;
double target_distance = 0;
int target_angle = 0;
int instructions[100];
int idx_instr = 0;
int idx_exec = 0;

void stop_robot() {
    motorFR->setSpeed(0);
    motorFL->setSpeed(0);
    motorRR->setSpeed(0);
    motorRL->setSpeed(0);
}

void clear_instructions() {
    for (int i=0; i<100; i++) {
        instructions[i] = 0;
    }
}

void decode_instruction() {
    // if(idx_exec >= idx_instr) {
    //     state = S_NONE;
    //     clear_instructions();
    //     idx_exec = 0;
    //     idx_instr = 0;
    //     return;
    // }
    switch(instructions[idx_exec]) {
        case(I_STOP):
            stop_robot();
            idx_exec++;
        break;
        case(I_SET_SPEED_FR):
            Serial.print("Speed FR set to ");
            Serial.println(instructions[idx_exec+1]);
            motorFR->setSpeed(instructions[idx_exec+1]);
            idx_exec += 2;
        break;
        case(I_SET_SPEED_FL):
            Serial.print("Speed FL set to ");
            Serial.println(instructions[idx_exec+1]);
            motorFL->setSpeed(instructions[idx_exec+1]);
            idx_exec += 2;
        break;
        case(I_SET_SPEED_RR):
            Serial.print("Speed RR set to ");
            Serial.println(instructions[idx_exec+1]);
            motorRR->setSpeed(instructions[idx_exec+1]);
            idx_exec += 2;
        break;
        case(I_SET_SPEED_RL):
            Serial.print("Speed RL set to ");
            Serial.println(instructions[idx_exec+1]);
            motorRL->setSpeed(instructions[idx_exec+1]);
            idx_exec += 2;
        break;
        case(I_MOV_DIST):
            target_distance = deadReckoner.getDistance() + instructions[idx_exec+1]*10;
            Serial.print("Target distance set to ");
            Serial.println(target_distance);
            motorFR->run(FORWARD);
            motorFL->run(FORWARD);
            motorRR->run(FORWARD);
            motorRL->run(FORWARD);
            state = S_WAIT_DISTANCE;
            idx_exec += 2;
        break;    
        case(I_TURN_RIGHT):
            target_angle = deadReckoner.getTheta() + instructions[idx_exec+1];
            Serial.print("Target angle set to ");
            Serial.println(target_distance);
            motorFR->run(BACKWARD);
            motorFL->run(FORWARD);
            motorRR->run(BACKWARD);
            motorRL->run(FORWARD);
            state = S_WAIT_ANGLE;
            idx_exec += 2;
        break;
        case(I_TURN_LEFT):
            target_angle = deadReckoner.getTheta() - instructions[idx_exec+1];
            Serial.print("Target angle set to ");
            Serial.println(target_distance);
            motorFR->run(FORWARD);
            motorFL->run(BACKWARD);
            motorRR->run(FORWARD);
            motorRL->run(BACKWARD);
            state = S_WAIT_ANGLE;
            idx_exec += 2;
        break;  
    }
}

int read_ultrasonic_sensor() {
    digitalWrite(p_trig, LOW);
    delayMicroseconds(2);
    digitalWrite(p_trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(p_trig, LOW);
	return(pulseIn(p_echo, HIGH) * 0.034 / 2);
}

void check_ultrasonic() {
    int distance = read_ultrasonic_sensor();
    if (distance < 20) {
        Serial.println("Too close");
        state = S_OBSTACLE;
        stop_robot();
    }
}
void pulseLeft() { leftTicks++; }
void pulseRight() { rightTicks++; }

void check_conditions() {
    switch(state) {
        case(S_NONE):
        break;
        case(S_NEXT_INSTR):
            decode_instruction();
        break;        
        case(S_WAIT_DISTANCE):
            if (deadReckoner.getDistance() >= target_distance) {
                stop_robot();
                state = S_NEXT_INSTR;
                target_distance = 0;
            }
        break;
        case(S_WAIT_ANGLE):
            if (deadReckoner.getTheta() >= target_distance) {
                stop_robot();
                state = S_NEXT_INSTR;
                target_distance = 0;
            }
        break;
    }
}

void setup() {
    // Motorshield
    Wire.begin(0x08);
    AFMS.begin();

    // encoders
    attachInterrupt(digitalPinToInterrupt(p_enc_l), pulseLeft, HIGH);
    attachInterrupt(digitalPinToInterrupt(p_enc_r), pulseRight, HIGH);
    
    // ultrasonic sensor
    pinMode(p_trig, OUTPUT); // Sets the trigPin as an Output
    pinMode(p_echo, INPUT); // Sets the echoPin as an Input

    clear_instructions();

    Serial.begin(115200);
    Serial.println("Ready.");
    test_motors_i2c();
}

void loop() {
    deadReckoner.update();
    check_conditions();
    check_ultrasonic();
    delay(1000);
    Serial.print("State: ");
    Serial.println(state);
    Serial.print("idx_instr: ");
    Serial.println(idx_instr);
    Serial.print("idx_exec: ");
    Serial.println(idx_exec);
}



void process_serial() {
    
}

void serialEvent() {
    while (Serial.available()) {
        int data_in = Serial.read();
        if (data_in == '.') {
            Serial.print("Received instructions: ");
            for(int i=0; i<idx_instr; i++) {
                Serial.print(instructions[i]);
                Serial.print(" ");  
            }
            Serial.println(" ");

            idx_exec = 0;
            idx_instr = 0;
            state = S_NEXT_INSTR;
            //idx_instr = 0;
            //decode_instruction();
            
        } else {
            instructions[idx_instr] = data_in;
            idx_instr++;
        }
    }
}

void test_motors_i2c() {
    int MAX = 100;
    Serial.println("Motor FR");
    Serial.println("Forward");
    motorFR->run(FORWARD);
    for (int i=0; i<MAX; i++) {
        motorFR->setSpeed(i);
        delay(1);
    }
    delay(3000);
    motorFR->run(RELEASE);
    motorFR->setSpeed(0);
    delay(1000);
    Serial.println("Backward");
    motorFR->run(BACKWARD);
    for (int i=0; i<MAX; i++) {
        motorFR->setSpeed(i);
        delay(1);
    }
    delay(3000);
    motorFR->run(RELEASE);
    motorFR->setSpeed(0);

    Serial.println("Motor FL");
    Serial.println("Forward");
    motorFL->run(FORWARD);
    for (int i=0; i<MAX; i++) {
        motorFL->setSpeed(i);
        delay(1);
    }
    delay(3000);
    motorFL->run(RELEASE);
    motorFL->setSpeed(0);
    delay(1000);
    Serial.println("Backward");
    motorFL->run(BACKWARD);
    for (int i=0; i<MAX; i++) {
        motorFL->setSpeed(i);
        delay(1);
    }
    delay(3000);
    motorFL->run(RELEASE);
    motorFL->setSpeed(0);

    Serial.println("Motor RR");
    Serial.println("Forward");
    motorRR->run(FORWARD);
    for (int i=0; i<MAX; i++) {
        motorRR->setSpeed(i);
        delay(1);
    }
    delay(3000);
    motorRR->run(RELEASE);
    motorRR->setSpeed(0);
    delay(1000);
    Serial.println("Backward");
    motorRR->run(BACKWARD);
    for (int i=0; i<MAX; i++) {
        motorRR->setSpeed(i);
        delay(1);
    }
    delay(3000);
    motorRR->run(RELEASE);
    motorRR->setSpeed(0);

    Serial.println("Motor RL");
    Serial.println("Forward");
    motorRL->run(FORWARD);
    for (int i=0; i<MAX; i++) {
        motorRL->setSpeed(i);
        delay(1);
    }
    delay(3000);
    motorRL->run(RELEASE);
    motorRL->setSpeed(0);
    delay(1000);
    Serial.println("Backward");
    motorRL->run(BACKWARD);
    for (int i=0; i<MAX; i++) {
        motorRL->setSpeed(i);
        delay(1);
    }
    delay(3000);
    motorRL->run(RELEASE);
    motorRL->setSpeed(0);
}
