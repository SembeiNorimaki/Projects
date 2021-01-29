#include "ValueIn.h"

#define I_GET_VAL     1
#define I_GET_MINMAX  2
#define I_SET_VAL     3
#define I_SET_MINMAX  4

int16_t read16() {
    byte h = Wire.read();  
    byte l = Wire.read();
    int16_t result = h << 8 + l;
    return result;
}

ValueIn::ValueIn(byte addr) {
    addr = addr;
}

void ValueIn::setVal(word val) {
    Wire.beginTransmission(addr);
    Wire.write(I_SET_VAL);
    Wire.write(highByte(val));
    Wire.write(lowByte(val));
    Wire.endTransmission();
}

void ValueIn::setMinMax(word minVal, word maxVal) {
    Wire.beginTransmission(addr);
    Wire.write(I_SET_MINMAX);
    Wire.write(highByte(minVal));
    Wire.write(lowByte(minVal));
    Wire.write(highByte(maxVal));
    Wire.write(lowByte(maxVal));
    Wire.endTransmission();
}

void ValueIn::getVal(word &val) {
    Wire.beginTransmission(addr);
    Wire.write(I_GET_VAL);
    Wire.endTransmission();

    Wire.requestFrom(addr, 2);
    val = read16();
}

void ValueIn::getMinMax(word &minVal, word &maxVal) {
    Wire.beginTransmission(addr);
    Wire.write(I_GET_MINMAX);
    Wire.endTransmission();

    Wire.requestFrom(addr, 4);
    minVal = read16();
    maxVal = read16();
}
