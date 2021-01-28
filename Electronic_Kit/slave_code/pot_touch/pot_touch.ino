/*
    Potentiometer + 2 touch buttons + Buzzer
*/

#include <Wire.h>
#include <EEPROM.h>

/**** Pin definitions *******/
#define P_POT     A0
#define P_TOUCH1  A3
#define P_TOUCH2  A2
#define P_BUZZER   4
/**** End of Pin definitions *******/

/*** Instructions ***/
#define I_NONE        0
#define I_GET_POT     1
#define I_GET_TOUCH   2
#define I_PING      254
#define I_ERR       255
/*** End of Instructions ***/

byte addr;
byte instr = I_NONE;
byte potVal = 0;
byte touchVal = 0;

void helloWorld() {
    beep(2);
}

void setup() {
    pinMode(P_TOUCH1, INPUT);
    pinMode(P_TOUCH2, INPUT);
    pinMode(P_POT, INPUT);
    pinMode(P_BUZZER, OUTPUT);
    
    // addr = EEPROM.read(0);
    addr = 2;
    Wire.begin(addr);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);

    delay(1000);
    helloWorld();
}

void loop() {
    readPot();
    readTouch();
    if bitRead(touchVal, 0) {
        tone(P_BUZZER, 10*potVal+1000);
    } else {
        noTone(P_BUZZER);
    }
}

int16_t read16() {
    byte h = Wire.read();  
    byte l = Wire.read();
    int16_t result = h << 8 + l;
    return result;
}

void receiveEvent(int n) {
    // Process instructions that don't require sending data back. Eg: setters
    instr = Wire.read();
    beep(instr+3);
    switch(instr) {
    }
}
void requestEvent() {
    // Process instructions requiring sending data back. Eg: getters
    switch(instr) {
        beep(instr+5);
        case(I_PING):
            Wire.write(I_PING);
        break;
        case(I_GET_POT):
            Wire.write(potVal);
        break;
        case(I_GET_TOUCH):
            Wire.write(touchVal);
        break;
        default:
            Wire.write(instr);
        break;
    }
    instr = I_NONE;
}

void readPot() {
    potVal = map(analogRead(P_POT), 0, 1023, 0, 100);
}
void readTouch() {
    bitWrite(touchVal, 0, digitalRead(P_TOUCH1));
    bitWrite(touchVal, 1, digitalRead(P_TOUCH2));
}

void beep(int n) {
    for (int i=0; i<n; i++) {
        tone(P_BUZZER, 1000);  // Send 1KHz sound signal...
        delay(100);           // ...for 1 sec
        noTone(P_BUZZER);      // Stop sound...
        delay(100);           // ...for 1sec
    }
}
