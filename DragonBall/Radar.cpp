#include "Radar.h"

Radar::Radar() : pixels(NPIXELS, LEDS_PIN, NEO_RGB + NEO_KHZ800) {
}

void Radar::initialize() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    // attachInterrupt(2, (void (*)())(&onButtonPress), 1);
    // attachInterrupt(2, Radar::onButtonRelease, 1);

    // loadTargets();
    // pixels.begin();
    // compass.init();
    // compass.setCalibration(-1463, 110, -48, 1396, -857, 0);
    EEPROM.begin(2 * NTARGETS * sizeof(double));

    //Serial.begin(9600);  // Serial is used for GPS
    // serialBT.begin("DragonBallRadar");

    //delay(10000);
    // serialBT << "Ready" << endl;
}

void Radar::btLedSeq(int ringId) {
    // set all pixels in the current ring
    pixels.clear();
    for (int i=OFFSET_RINGS[ringId]; i<OFFSET_RINGS[ringId]+RINGS[ringId]-1; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 40));  // blue
    }
    pixels.show();
}

void Radar::bluetoothCommunication() {  
    // 1. initialize the bluetooth port
    mode = M_BLUETOOTH;
    serialBT.begin("DragonBallRadar");
    int start_bluetooth_ms = millis();
    
    int ringId = 0;
    unsigned long last_led_change_ms = millis();
    String rx = "";

    while(rx != "pong") {
        // Timeout
        if (millis() - start_bluetooth_ms > BT_TIMEOUT_MS) {
            serialBT << "Timed out" << endl;
            delay(100);
            serialBT.end();
            mode = M_NORMAL;
            return;
        }
        // led wave figure
        if (millis() - last_led_change_ms > 1000) {
            serialBT << "ping" << endl;
            btLedSeq(ringId++);
            if (ringId >= NRINGS)
                ringId = 0;
            last_led_change_ms = millis();
        }
        
        // 2. Wait until it receives pong
        while (serialBT.available() > 0) {
             rx += serialBT.readString();
        }
    }
    sendTargets();
    //serialBT << "Success" << endl;
    delay(100);
    // 3. Process the data
    //decodeRX();
    // 4. Disable bluetooth
    serialBT.end();
    mode = M_NORMAL;
}

void IRAM_ATTR Radar::onButtonPress() {
    buttonPressed = true;
    displayON = true;
    last_press_ms = millis();
}

void IRAM_ATTR Radar::onButtonRelease() {
    buttonPressed = false;
}

void Radar::loadTargets() {
    // Loads the targets from the EEPROM
    int addr = 0;
    double lat, lon;
    for (int i=0; i<NTARGETS; i++) {
        EEPROM.get(addr, lat);
        addr += sizeof(double);
        EEPROM.get(addr, lon);
        addr += sizeof(double);
        setTarget(i, lat, lon);
    }
}

void Radar::storeTarget(int id, double lat, double lon) {
    // Stores a target into the EEPROM
    // each target stores latitude and longitude as double
    // id: 0 -> addr: 0
    // id: 1 -> addr: 2*sizeof(double)
    // id: 2 -> addr: 4*sizeof(double)

    int addr = id * 2 * sizeof(double);
    EEPROM.put(addr, lat);
    addr += sizeof(double);
    EEPROM.put(addr, lon);
    EEPROM.commit();
    delay(10);
}

void Radar::setTarget(int id, double lat, double lon) {
    targets[id].latitude = lat;
    targets[id].longitude = lon;
    // also update distance and angle
    targets[id].distance = (unsigned long)TinyGPSPlus::distanceBetween(
        MyPosition[0], MyPosition[1], lat, lon);
    targets[id].angle = TinyGPSPlus::courseTo(
        MyPosition[0], MyPosition[1], lat, lon);
}

void Radar::sendTargets() {
    StaticJsonDocument<200> doc;
    for (int i=0; i<NTARGETS; i++) {
        JsonArray data = doc.createNestedArray(String(i));
        data.add(targets[i].latitude);
        data.add(targets[i].longitude);
    }
    serializeJson(doc, serialBT);
}

void Radar::decodeRX() {
    char dataRx[] = "{\"numTargets\": 3,\"targets\":[[11,22],[33,44],[55,66]]}";
    DynamicJsonDocument doc(1000);
    DeserializationError error = deserializeJson(doc, dataRx);

    // Test if parsing succeeds.
    if (error) {
        serialBT.print(F("deserializeJson() failed: "));
        serialBT.println(error.c_str());
        return;
    }

    int n = doc["numTargets"];
    for (int i=0; i<n; i++) {
        double lat = doc["targets"][i][0];
        double lon = doc["targets"][i][1];
    }
}

void Radar::update(bool debug) {
    if (!displayON) {
        return;
    }
    if ((buttonPressed) && (millis() - last_press_ms > LONGPRESS_MS)) {
        // We have pressed the button for longer than LONGPRESS_MS
        // Enter DEBUG or PAIRING MODE

    }
    if (millis() - last_press_ms > DISPLAY_MS) {
        displayON = false;
        return;
    }
    
    scan();
    if (debug) {
        serialBT << "My location: (" << _FLOAT(MyPosition[0], 6) << ", " 
                                     << _FLOAT(MyPosition[1], 6) << ")" << endl;
        for(int i=0; i<NTARGETS; i++) {
            serialBT << "Target " << i << ":    (" 
                     << _FLOAT(targets[i].latitude, 6) << ", " 
                     << _FLOAT(targets[i].longitude, 6) << ")" << endl;
            serialBT << "Distance: " << targets[i].distance 
                     << " Angle: " << targets[i].angle 
                     << " AngleCorr: " << targets[i].angleCorrected  
                     << " LedId: " << targets[i].ledId << endl;
        }
        serialBT << "Compass Angle: " << compassAngle << endl;
        serialBT << "************************" << endl;
    }
    delay(100);
}

int Radar::distance2ring(int distance) {
    // target out of range returns outer ring
    if (distance > RING_MAX_DIST[0])  
        return 0; // outer ring

    for (int i=1; i<NRINGS; i++) {
        if (distance > RING_MAX_DIST[i]) {
            return i-1;
        }
    }
    // target closer than last RING_MAX_DIST value returns the most inner ring
    return NRINGS-1; 
}

void Radar::updateLedId(int targetId, int distance, int angle_) {
    // angle must be in the range [0..360)
    int ringId = distance2ring(distance);
    int nleds = RINGS[ringId];
    int ledIdAux = floor(0.5 + nleds * angle_ / 360);
    if (ledIdAux == nleds) {
        ledIdAux = 0;
    }
    targets[targetId].ledId = ledIdAux + OFFSET_RINGS[ringId];
    // serialBT << "Ringid: " << ringId 
    //          << " ledIdAux: " << ledIdAux 
    //          << " ledId: " << targets[targetId].ledId << endl;
}

void Radar::updateGpsLocation() {
    // read gps and update the position
    while (Serial.available() > 0) {
        gps.encode(Serial.read());
        yield();  // not sure if yield here will make it not work
    }
    if (gps.location.isUpdated()) {
        MyPosition[0] = gps.location.lat();
        MyPosition[1] = gps.location.lng();

        // recalculate distances and angles to all targets
        for (int i=0; i<NTARGETS; i++) {
            targets[i].distance = (unsigned long)TinyGPSPlus::distanceBetween(
                MyPosition[0], MyPosition[1], targets[i].latitude, targets[i].longitude);
            targets[i].angle = TinyGPSPlus::courseTo(
                MyPosition[0], MyPosition[1], targets[i].latitude, targets[i].longitude);
        }

        // serialBT << "Lat: " << _FLOAT(MyPosition[0], 6) 
        //          << " Lon: " << _FLOAT(MyPosition[1], 6) 
        //          << " Nsat: " << gps.satellites.value() << endl;
    }
}

void Radar::scan() {
    // update gps if needed
    if (millis() - last_gps_ms > GPS_MS) {
        Radar::updateGpsLocation();
        last_gps_ms = millis();
    }
    // update compassAngle
    compass.read();
    compassAngle = compass.getAzimuth();
    // check that compassAngle is in the range [0, 360)
    // otherwise set angle to 0 so radar acts in cardinal mode
    if ((compassAngle < 0) || (compassAngle >= 360)) {
        compassAngle = 0;
        //serialBT << "Error reading Compass" << endl;
    }
    // update ledID for all targets
    for (int i=0; i<NTARGETS; i++) {
        if (compassEnabled) {
            targets[i].angleCorrected = targets[i].angle - compassAngle;
            if (targets[i].angleCorrected < 0) {
                targets[i].angleCorrected += 360;  
            } 
        } else {
            targets[i].angleCorrected = targets[i].angle;
        }
        updateLedId(i, targets[i].distance, targets[i].angleCorrected);
    }
    blinkLed();
}

void Radar::blinkLed() {  
    if (millis() - last_blink_ms > BLINK_MS) {
        if (ledStatus) {
            pixels.clear();
            // for(int i=0; i<NPIXELS; i++) {
            //     pixels.setPixelColor(i, pixels.Color(0, 0, 0));
            // }
            pixels.show();
            ledStatus = false;
        } else {
            for (int i=0; i<NTARGETS; i++) {
                if (showUnreachable && (targets[i].distance >= RING_MAX_DIST[0])) {
                    pixels.setPixelColor(targets[i].ledId, pixels.Color(40, 0, 0));  // red
                } else {
                    pixels.setPixelColor(targets[i].ledId, pixels.Color(0, 40, 0));  // green
                }
            }
            pixels.show();
            ledStatus = true;
        }
        last_blink_ms = millis();
    }
}


/***************** TEST FUNCTIONS *********************/

void Radar::testLeds() {
    for(int dist=450; dist>0; dist -= 100) { // 10
        for(int angle=0; angle<360; angle+=10) {  // 36
            //serialBT << "Dist: " << dist << " Angle: " << angle << endl;
            updateLedId(0, dist, angle);
            pixels.setPixelColor(targets[0].ledId, pixels.Color(0, 40, 0));  // green 
            pixels.show();
            delay(500);
            pixels.clear();
            // for(int i=0; i<NPIXELS; i++) {
            //     pixels.setPixelColor(i, pixels.Color(0, 0, 0));
                
            //     //delay(10);
            // }
            pixels.show();
            
            delay(500);
        }
    }
}

void Radar::testGps() {
    while (Serial.available() > 0) {
        serialBT.write(Serial.read());
    }
}

void Radar::testCompass() {
    compass.read();
    serialBT << "Azimuth: " << compass.getAzimuth() << endl;
    delay(500); 
}

void Radar::testEeprom() {
    double wLat = 47.384198;
    double wLon = 8.508892;
    double rLat, rLon; 
    int addr = 0;

    EEPROM.put(addr, wLat);
    addr += sizeof(double);
    EEPROM.put(addr, wLon);
    
    EEPROM.commit();
    delay(200);

    addr = 0;
    EEPROM.get(addr, rLat);
    addr += sizeof(double);
    EEPROM.get(addr, rLon);

    serialBT << "Written: (" << _FLOAT(wLat, 6) << ", " << _FLOAT(wLon, 6) << ")" << endl; 
    serialBT << "Read:    (" << _FLOAT(rLat, 6) << ", " << _FLOAT(rLon, 6) << ")" << endl; 
}

void Radar::testParser() {
    //String JSONMessage = serialBT.readString();
   
    DynamicJsonDocument doc(1024);
    char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

    DeserializationError error = deserializeJson(doc, json);

    // Test if parsing succeeds.
    if (error) {
        serialBT.print(F("deserializeJson() failed: "));
        serialBT.println(error.c_str());
        return;
    }

    // Fetch values.
    //
    // Most of the time, you can rely on the implicit casts.
    // In other case, you can do doc["time"].as<long>();
    const char* sensor = doc["sensor"];
    long time = doc["time"];
    double latitude = doc["data"][0];
    double longitude = doc["data"][1];

    // Print values.
    serialBT.println(sensor);
    serialBT.println(time);
    serialBT.println(latitude, 6);
    serialBT.println(longitude, 6);
}

void Radar::test() {
    if (testId == 0) {
        testGps();
    } else if (testId == 1) {
        testCompass();
    } else if (testId == 2) {
        testLeds();
    } else {
        serialBT << "Unknown command" << endl;
    }

    while (serialBT.available() > 0) {
        testId = serialBT.read() - '0';
    }
}
