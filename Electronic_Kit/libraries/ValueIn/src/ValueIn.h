#ifndef _ValueIn_h
#define _ValueIn_h

#include <Arduino.h>
#include <Wire.h>

class ValueIn {
public:
    ValueIn(byte addr);
    
    void setVal(word val);
    void setMinMax(word minVal, word maxVal);
    void getVal(word &val);
    void getMinMax(word &minVal, word &maxVal);

private:
    byte addr;
};
#endif
