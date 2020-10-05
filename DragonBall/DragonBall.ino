#include "Radar.h"

Radar radar;


void setup() {
    Serial.begin(115200);
    radar.initialize();
}

void loop() {
    radar.update(false);
    //radar.testLeds();
    //radar.testGps();
    //radar.testCompass();
    //delay(10000);
}
