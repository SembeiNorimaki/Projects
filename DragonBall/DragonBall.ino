#include "Radar.h"

Radar radar;


void setup() {
    Serial.begin(9600);
    radar.initialize();
}

void loop() {
    //radar.update(false);
    // radar.testLeds();
    //radar.testEeprom();
    radar.testParser();
    //radar.testGps();
    //radar.testCompass();
    //radar.test();
    delay(5000);
}
