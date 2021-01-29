/*
    8x8 Matrix
*/

#include <Wire.h>
#include <EEPROM.h>

/**** Pin definitions *******/
#define P_COL_ADDR0  1
#define P_COL_ADDR1  2
#define P_COL_ADDR2  3
#define P_ROW_ADDR0  1
#define P_ROW_ADDR1  2
#define P_ROW_ADDR2  3
#define P_EN         4
/**** End of Pin definitions *******/

/*** Instructions ***/
#define I_NONE    0
#define I_SET_PX  1
#define I_SET_ROW 2
#define I_SET_MAT 3
#define I_GET_DATA 4

#define I_PING  254
#define I_ERR   255
/*** End of Instructions ***/

byte addr;
byte instr = I_NONE;
byte data[8] = {0, 0, 0, 0, 0, 0, 0, 0};  // each index is a row


void helloWorld() {

}

void setup() {
    addr = 4;
    // addr = EEPROM.read(0);
    Wire.begin(addr);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);

    delay(1000);
    helloWorld();
}

void loop() {
    refresh();
}

void refresh() {
    for (byte i=0; i<8; i++) {
        digitalWrite(P_EN, LOW);
        digitalWrite(P_ROW_ADDR0, bitRead(i, 0));
        digitalWrite(P_ROW_ADDR1, bitRead(i, 1));
        digitalWrite(P_ROW_ADDR2, bitRead(i, 2));

        for (byte j=0; j<8; j++) {
            digitalWrite(P_EN, LOW);
            digitalWrite(P_COL_ADDR0, bitRead(j, 0));
            digitalWrite(P_COL_ADDR1, bitRead(j, 1));
            digitalWrite(P_COL_ADDR2, bitRead(j, 2));
            digitalWrite(P_EN, bitRead(data[i], j));
        }
    }
}
int16_t read16() {
    byte h = Wire.read();  
    byte l = Wire.read();
    int16_t result = h << 8 + l;
    return result;
}

void receiveEvent(int n) {
    instr = Wire.read();
    byte row, col, val;
    switch(instr) {
        case(I_SET_PX):
            row = Wire.read();
            col = Wire.read();
            val = Wire.read();
            bitWrite(data[row], col, bool(val));
            instr = I_NONE;
        break;
        case(I_SET_ROW):
            row = Wire.read();
            val = Wire.read();
            data[row] = val;
            instr = I_NONE;
        break;
        case(I_SET_MAT):
            for (int i=0; i<8; i++)
                data[i] = Wire.read();
            instr = I_NONE;
        break;
    }
}

void requestEvent() {
    switch(instr) {
        case(I_PING):
            Wire.write(I_PING);
        break;
        case(I_GET_DATA):
            for (int i=0; i<8; i++)
                Wire.write(data[i]);
        break;
        
        default:
            Wire.write(I_ERR);
        break;
    }
    instr = I_NONE;
}
