#include <Wire.h>
#include <Adafruit_MotorShield.h>

// encoders
#define p_enc_l 3

#define N 30
double difference[N];
int idx=0;
unsigned long changeTime = 0;
unsigned long sendTime = 0;
unsigned long tickTime;


Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *motorFL = AFMS.getMotor(1);

void pulseLeft() { 
    tickTime = micros();
    difference[idx] = tickTime - changeTime;
    idx++;
    if(idx >= N) {
        idx = 0;
    }
    changeTime = tickTime;
}

void setup() {
    // Motorshield
    Wire.begin(0x08);
    AFMS.begin();

    // encoders
    attachInterrupt(digitalPinToInterrupt(p_enc_l), pulseLeft, RISING);

    Serial.begin(115200);

    motorFL->run(FORWARD);
    for (int i=0; i<50; i++) {
        motorFL->setSpeed(i);
        delay(1);
    }
}

void loop() {
    if (millis() - sendTime > 1000) {
        for(int i=0; i<N; i++) {
            Serial.print(difference[i]);
            Serial.print(" ");
        }
        Serial.println("");
        
        sendTime = millis();
    }
}