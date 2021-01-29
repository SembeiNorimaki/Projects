#include <Mat5x5.h>
#include <Color.h>

mat = Mat5x5();

void setup() {
    
}

void loop() {
    for (int i=0; i<mat.NPIXELS; i++) {
        mat.setPixel(i, Color(100, 0, 0));
        delay(200);
    }    
}
