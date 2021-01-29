#include <Wire.h>
#include <EEPROM.h>

/**** Pin definitions *******/
#define P_ENC_LEFT   2
#define P_ENC_RIGHT  3
#define P_ENC_BUTTON 4
/**** End of Pin definitions *******/

/*** Instructions ***/
#define I_NONE        0

#define I_PING      254
#define I_ERR       255
/*** End of Instructions ***/


byte addr = 11;
byte instr = I_NONE;

int16_t val = 0;
int16_t minVal = 0;
int16_t maxVal = 100;

void setup() {
    Wire.begin(addr);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);

    attachInterrupt(digitalPinToInterrupt(P_ENC_LEFT), onPulseLeft, HIGH);
    attachInterrupt(digitalPinToInterrupt(P_ENC_RIGHT), onPulseRight, HIGH);
}

void loop() {

}

void onPulseLeft() {
    val--;
    if (val < minVal)
        val = minVal;
}

void onPulseRight() {
    val++;
    if (val > maxVal)
        val = maxVal;
}

void receiveEvent(int n) {
    instr = Wire.read();
    blink(instr);
    switch(instr) {
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