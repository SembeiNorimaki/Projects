#include <Wire.h>
#include <EEPROM.h>


/**** Pin definitions *******/
#define P_RED 7
#define P_YEL 6
#define P_GRN 5
/**** End of Pin definitions *******/

/*** Instructions ***/
#define I_NONE        0
#define I_TURN_ON     1
#define I_TURN_OFF    2
#define I_SET_TIMER   3
#define I_SET_MODE    4
#define I_PING      254
#define I_ERR       255
/*** End of Instructions ***/

#define LED_GRN 1
#define LED_YEL 2
#define LED_RED 3

#define M_AUTO   1
#define M_MANUAL 2

byte addr;
byte instr = I_NONE;
byte runMode = M_MANUAL;
byte state = LED_RED;

int16_t timers[3] = {1000, 1000, 1000};

unsigned long lastMillis = 0;

void helloWorld() {
    turn_on(LED_GRN);
    delay(300);
    turn_on(LED_YEL);
    delay(300);
    turn_on(LED_RED);
    delay(300);
    turn_off();
}

void blink(byte n) {
    for (byte i=0; i<n; i++) {
        turn_on(LED_GRN);
        delay(200);
        turn_on(LED_RED);
        delay(200);
    }
    turn_off();
}

void setup() {
    pinMode(P_RED, OUTPUT);
    pinMode(P_YEL, OUTPUT);
    pinMode(P_GRN, OUTPUT);
    //addr = EEPROM.read(0);
    addr = 3;
    Wire.begin(addr);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
    
    delay(1000);
    helloWorld();
}

void loop() {
    if (runMode == M_AUTO) {
        if ((state == LED_RED) && (millis() - lastMillis >= timers[2])) {
            turn_on(LED_GRN);
            lastMillis = millis();
        } else if ((state == LED_YEL) && (millis() - lastMillis >= timers[1])) {
            turn_on(LED_RED);
            lastMillis = millis();
        } else if ((state == LED_GRN) && (millis() - lastMillis >= timers[0])) {
            turn_on(LED_YEL);
            lastMillis = millis();
        }
    }
}

void turn_on(byte led) {
    digitalWrite(P_GRN, led == LED_GRN);
    digitalWrite(P_YEL, led == LED_YEL);
    digitalWrite(P_RED, led == LED_RED);
    state = led;
}

void turn_off() {
    digitalWrite(P_GRN, LOW);
    digitalWrite(P_YEL, LOW);
    digitalWrite(P_RED, LOW);
    state = 0;
}

int16_t read16() {
    byte h = Wire.read();  
    byte l = Wire.read();
    int16_t result = h << 8 + l;
    return result;
}

void receiveEvent(int n) {
    instr = Wire.read();
    switch(instr) {
        case(I_TURN_ON):
            turn_on(Wire.read());
            instr = I_NONE;
        break;
        case(I_TURN_OFF):
            turn_off();
            instr = I_NONE;
        break;
        case(I_SET_TIMER):
            byte led = Wire.read();
            timers[led] = read16();
            instr = I_NONE;
        break;
        case(I_SET_MODE):
            runMode = Wire.read();
            instr = I_NONE;
        break;
    }
}

void requestEvent() {
    switch(instr) {
        case(I_PING):
            Wire.write(I_PING);
        break;
        
        default:
            Wire.write(I_ERR);
        break;
    }
    instr = I_NONE;
}
