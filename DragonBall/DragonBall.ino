#include "Radar.h"

Radar radar;


void setup() {
    Serial.begin(9600);
    radar.initialize();

    radar.storeTarget(0, 47.384196, 8.508903);
    delay(100);
    radar.loadTargets();
    //radar.bluetoothCommunication();
    // radar.failureLeds();
    // radar.successLeds();

}

void loop() {
    radar.update(false);
    
    
    // radar.testLeds();
    //radar.testEeprom();
    //radar.testParser();
    //radar.testGps();
    //radar.testCompass();
    //radar.test();
    //delay(5000);
}
