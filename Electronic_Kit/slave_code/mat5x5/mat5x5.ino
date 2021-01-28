/*
    5x5 RGB Matrix
*/

#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <EEPROM.h>

/**** Pin definitions *******/
#define P_PIXELS 3
/**** End of Pin definitions *******/

/*** Instructions ***/
#define I_NONE        0
#define I_SET_PX      1
#define I_SET_MAT     2
#define I_PING      254
#define I_ERR       255
/*** End of Instructions ***/

#define NUMPIXELS 25

Adafruit_NeoPixel pixels(NUMPIXELS, P_PIXELS, NEO_GRB + NEO_KHZ800);
byte addr;
byte instr = I_NONE;

void helloWorld() {
    for(int i=0; i<NUMPIXELS; i++) {
        pixels.clear();
        pixels.setPixelColor(i, pixels.Color(0, 150, 0));
        pixels.show();  
        delay(100);
    }
    pixels.clear();
}

void setup() {
    // addr = EEPROM.read(0);
    addr = 3;
    pixels.begin();
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
        case(I_SET_PX):
            // Sets pixel to RGB color
            byte px_idx = Wire.read(); 
            byte r = Wire.read();
            byte g = Wire.read();
            byte b = Wire.read();
            pixels.setPixelColor(px, pixels.Color(r, g, b));
            pixels.show();
            instr = I_NONE;
        break;
        case(I_SET_MAT):
            // Sets all pixels in the matrix
            byte px, r, g, b;
            for (int i=0; i<NUMPIXELS; i++) {
                px = Wire.read(); 
                r = Wire.read();
                g = Wire.read();
                b = Wire.read();
                pixels.setPixelColor(px, pixels.Color(r, g, b));
            }
            pixels.show();
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
