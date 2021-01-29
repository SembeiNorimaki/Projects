#include "Mat5x5.h"

#define I_SET_PIXEL  1
#define I_SET_PIXELS 2
#define NUMPIXELS 25

Color::Color(byte r, byte g, byte b) {
    r = r;
    g = g;
    b = b;
}

Mat5x5::Mat5x5() {
}

void Mat5x5::setPixel(byte idx, Color c) {
    Wire.beginTransmission(addr);
    Wire.write(I_SET_PIXEL);
    Wire.write(idx);
    Wire.write(c.r);
    Wire.write(c.g);
    Wire.write(c.b);
    Wire.endTransmission();
}

void Mat5x5::setPixels(std::vector<Color> c) {
    Wire.beginTransmission(addr);
    Wire.write(I_SET_PIXELS);
    for (int i=0; i<c.size(); i++) {
        Wire.write(c[i].r);
        Wire.write(c[i].g);
        Wire.write(c[i].b);
    }
    Wire.endTransmission();
}
