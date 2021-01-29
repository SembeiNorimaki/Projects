#ifndef _Mat5x5_h
#define _Mat5x5_h

#include <Arduino.h>
#include <Wire.h>
#include <iostream>
#include <vector>

class Color {
public:
    Color(byte r, byte g, byte b);
    byte r, g, b;
};

class Mat5x5 {
public:
    const byte NPIXELS = 25;

    Mat5x5(); 
    void setPixel(byte idx, Color c);
    void setPixels(std::vector<Color> c);

private:
    byte addr = 3;
};
#endif
