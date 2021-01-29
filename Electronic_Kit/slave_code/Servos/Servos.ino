#include <Wire.h>
#include <EEPROM.h>


/**** Pin definitions *******/
#define P_SERVOX 3
#define P_SERVOY 4
/**** End of Pin definitions *******/

/*** Instructions ***/
#define I_NONE     0
#define I_SET_X    1
#define I_SET_Y    2
#define I_PING   254
#define I_ERR    255
/*** End of Instructions ***/

byte addr;
byte instr = I_NONE;

void helloWorld() {
}

void setup() {
    //pinMode()
    addr = EEPROM.read(0);
    Wire.begin(addr);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);

    delay(1000);
    helloWorld();
}

void loop() {
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
        case(I_SET_X):
        break;
        case(I_SET_Y):
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
