#include "Robot.h"
#include <DeadReckoner.h>
#include <Streaming.h>
// #include <AutoPID.h>

#define STX '['             // start transmission character
#define ETX ']'             // end transmission character
#define SEP ';'
#define MAXTIME 5000        // max time without receiving data

#define RADIUS 60.0         // wheel radius in mm
#define LENGTH 220.0        // wheel base length in mm
#define TICKS_PER_REV 770.0

// TIME INTERVALS
#define POSITION_COMPUTE_INTERVAL_MS 50
#define POSITION_SEND_INTERVAL_MS 1000

// states
#define S_NONE 0
#define S_NEXT_INSTR 1
#define S_WAIT_DISTANCE 2
#define S_WAIT_ANGLE 3
#define S_OBSTACLE 4

// PID settings and gains
// #define OUTPUT_MIN 0
// #define OUTPUT_MAX 255
// #define KP .12
// #define KI .0003
// #define KD 0

// encoders
#define p_enc_l 2
#define p_enc_r 3

// ultrasonic sensor
#define p_trig 9
#define p_echo 10

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// PID
// double currentLeftSpeed, targetLeftSpeed, outputLeftPID;
// double currentRightSpeed, targetRightSpeed, outputRightPID;

// AutoPID leftPID(&currentLeftSpeed, &targetLeftSpeed, &outputLeftPID, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);
// AutoPID rightPID(&currentRightSpeed, &targetRightSpeed, &outputRightPID, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);

volatile unsigned long leftTicks, rightTicks;
DeadReckoner dR = DeadReckoner(&leftTicks, &rightTicks, POSITION_COMPUTE_INTERVAL_MS, POSITION_SEND_INTERVAL_MS, 
                               TICKS_PER_REV, RADIUS, LENGTH);

Robot robot = Robot();

int state = S_NONE;
double target_distance = 0;
int target_angle = 0;

char data_in[100];
int idx = 0;
int idx_exec = 0;
unsigned long lastSerial = millis();

// Instructions:
// I_STOP 
// I_SET_SPEED, SPEED
// I_MOV_DIST, DIST  (sets state to S_WAIT_DISTANCE)
// I_TURN_DEG, ANGLE (sets state to S_WAIT_ANGLE)
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
            robot.set_speed(0, 0);
            idx_exec++;
        break;
        case(I_SET_SPEED):
            robot.set_speed(instructions[idx_exec+1], instructions[idx_exec+2]);
            idx_exec += 3;
        break;
        case(I_MOV_DIST):
            target_distance = deadReckoner.getDistance() + instructions[idx_exec+1]*10;
            state = S_WAIT_DISTANCE;
            idx_exec += 2;
        break;    
        case(I_TURN_DEG):
            if (instructions[idx_exec+1] > 0) {
                robot.set_speed(60, -60);   // turn clockwise
            } else {
                robot.set_speed(-60, 60);   // turn anticlockwise
            }
            target_angle = deadReckoner.getTheta() + instructions[idx_exec+1];
            state = S_WAIT_ANGLE;
            idx_exec += 2;
        break;
    }
}

// STATES:
// S_NONE
// S_WAIT_DISTANCE
// S_WAIT_ANGLE
void check_conditions() {
    switch(state) {
        case(S_NONE):
        break;
        case(S_NEXT_INSTR):
            decode_instruction();
        break;        
        case(S_WAIT_DISTANCE):
            if (deadReckoner.getDistance() >= target_distance) {
                robot.set_speed(0, 0);
                state = S_NEXT_INSTR;
                target_distance = 0;
            }
        break;
        case(S_WAIT_ANGLE):
            if (deadReckoner.getTheta() >= target_angle) {  // TODO: this wont work 
                robot.set_speed(0, 0);
                state = S_NEXT_INSTR;
                target_angle = 0;
            }
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


void setup() {
    pinMode(p_trig, OUTPUT);
    pinMode(p_echo, INPUT);
    
    // Serial.begin(115200);
    Serial.begin(9600);
    Wire.begin(0x08);
    AFMS.begin();
    robot.initialize(AFMS);

    // encoders
    attachInterrupt(digitalPinToInterrupt(p_enc_l), pulseLeft, HIGH);
    attachInterrupt(digitalPinToInterrupt(p_enc_r), pulseRight, HIGH);
    
    // set PID update interval
    // leftPID.setTimeStep(4000);
    // rightPID.setTimeStep(4000);

    //delay(2000);
    //Serial << "Ready" << endl;
}

void loop() {
    dR.update();
    check_conditions();

    // Serial << read_ultrasonic_sensor() << endl;
    // delay(500);

    // currentLeftSpeed = dR.getWl();
    // currentRightSpeed = dR.getWr();
    // leftPID.run();
    // rightPID.run();
    // Serial << "LeftOut " << outputLeftPID << " RightOut " << outputRightPID << endl;
    // Serial << "LeftSpeed " << currentLeftSpeed << " RightSpeed " << currentRightSpeed << endl;
    // Serial << "TargetLeftSpeed " << targetLeftSpeed << " TargetRightSpeed " << targetRightSpeed << endl;
    
    //robot.set_speed(outputLeftPID, outputRightPID);
    //delay(500);
    
    // if we dont receive serial data for X millis -> stop
    //if (millis() - lastSerial > MAXTIME) {
        //Serial.println("Timeout. Robot stopped.");
    //    robot.set_speed(0, 0);
    //}
}

void pulseLeft() { 
    leftTicks++; 
}

void pulseRight() { 
    rightTicks++; 
}

void clean_rx() {
    for(int i=0; i<100; i++)
        data_in[i] = 0;
    idx = 0;
}

// array of characters separated by ;
// parses floats delimited by ;
// decodes the floats as 
// [n;instr,param]
void process_serial2() {
    char *rest = data_in;
    double n = atof(strtok_r(rest, SEP, &rest));
    double b = atof(strtok_r(rest, SEP, &rest));

    //Serial << "Setting robot speed to (" << a << ", " << b << ")" << endl;
    robot.set_speed(a, b);
    clean_rx();
}

// array of characters separated by ;
// parses floats delimited by ;
// decodes the two first floats as speed_left, speed_right
void process_serial() {
    // Serial.println("Processsing serial");
    // targetLeftSpeed = data_in[0];
    // targetRightSpeed = data_in[1];
    
    char *rest = data_in;
    double a = atof(strtok_r(rest, SEP, &rest));
    double b = atof(strtok_r(rest, SEP, &rest));

    //Serial << "Setting robot speed to (" << a << ", " << b << ")" << endl;
    robot.set_speed(a, b);
    clean_rx();
}

// saves anything between [ and ] into data_in as character
//eg: [2.3;4.5] -> data_in = ["2", ".", "3", ";", "4", ".", "5"]
void serialEvent() {
    while (Serial.available()) {
        char aux = (char)Serial.read();
        //Serial << aux + 1 << endl;
        //Serial << "Received " << aux << endl;
        if (aux == STX) {
            idx = 0;
        } else if (aux == ETX) {
            process_serial();
        } else {
            data_in[idx] = aux;
            idx++;
        }
        lastSerial = millis();
    }
}
