#include "Arduino.h"
#include "Radar.h"

//Radar::Radar() : pixels(NPIXELS, LEDS_PIN, NEO_GRB + NEO_KHZ800), serialGPS(GPS_RX, GPS_TX) { 
Radar::Radar() : pixels(NPIXELS, LEDS_PIN, NEO_GRB + NEO_KHZ800), serialGPS(GPS_RX, GPS_TX){
}

void Radar::initialize() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    initializeTargets();
    pixels.begin();
    compass.init();
    compass.setCalibration(-1463, 110, -48, 1396, -857, 0);

    #ifdef DEVICE_ESP01
      serialGPS.begin(GPS_BAUD, SWSERIAL_8N1, GPS_RX, GPS_TX, false, 95, 11);
    #endif
    #ifdef DEVICE_ESP32
      serialGPS.begin(GPS_BAUD, SWSERIAL_8N1, GPS_RX, GPS_TX, false, 95, 11);
    #endif
    #ifdef DEVICE_ARDUINO
      serialGPS.begin(GPS_BAUD);
    #endif
    Serial << "Ready" << endl;
}

void Radar::initializeTargets() {
    setTarget(0, 47.393890, 8.520015);
}

void Radar::update(bool debug) {
    scan();
    if (debug) {
        Serial << "My location: (" << _FLOAT(MyPosition[0], 6) << ", " 
                                   << _FLOAT(MyPosition[1], 6) << ")" << endl;
        for(int i=0; i<NTARGETS; i++) {
            Serial << "Target " << i << ":    (" 
                   << _FLOAT(targets[i].latitude, 6) << ", " 
                   << _FLOAT(targets[i].longitude, 6) << ")" << endl;
            Serial << "Distance: " << targets[i].distance 
                   << " Angle: " << targets[i].angle 
                   << " AngleCorr: " << targets[i].angleCorrected  
                   << " LedId: " << targets[i].ledId << endl;
        }
        Serial << "Compass Angle: " << compassAngle << endl;
        Serial << "************************" << endl;
    }
    delay(100);
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

// OK
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

// OK
void Radar::updateLedId(int targetId, int distance, int angle_) {
    // angle must be in the range [0..360)
    int ringId = distance2ring(distance);
    int nleds = RINGS[ringId];
    int ledIdAux = floor(0.5 + nleds * angle_ / 360);
    if (ledIdAux == nleds) {
        ledIdAux = 0;
    }
    targets[targetId].ledId = ledIdAux + OFFSET_RINGS[ringId];
    // Serial << "Ringid: " << ringId 
    //        << " ledIdAux: " << ledIdAux 
    //        << " ledId: " << targets[targetId].ledId << endl;
}

void Radar::updateGpsLocation() {
    // read gps and update the position
    // while (SerialGPS.available() > 0) {
    //     gps.encode(SerialGPS.read());
    //     //yield();
    // }
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

        // Serial << "Lat: " << _FLOAT(MyPosition[0], 6) 
        //        << " Lon: " << _FLOAT(MyPosition[1], 6) 
        //        << " Nsat: " << gps.satellites.value() << endl;
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
        Serial << "Error reading Compass" << endl;
        delay(500);
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

void Radar::testLeds() {
    for(int dist=450; dist>0; dist -= 100) { // 10
        for(int angle=0; angle<360; angle+=10) {  // 36
            Serial << "Dist: " << dist << " Angle: " << angle << endl;
            updateLedId(0, dist, angle);
            pixels.setPixelColor(targets[0].ledId, pixels.Color(0, 40, 0));  // green 
            pixels.show();
            delay(500);
            pixels.clear();
            pixels.show();
        }
    }
}

void Radar::testGps() {
    while (serialGPS.available() > 0) {
        Serial.write(serialGPS.read());
    }
}

void Radar::testCompass() {
    compass.read();
    Serial << "Azimuth: " << compass.getAzimuth() << endl;
    delay(500); 
}
