/* Radar sequence:
*  - When powered on:
*    - Wait for GPS signal
*    - Check compass
*
*
*/

#include "Radar.h"
//#include "GPS.h"
//#include "Compass.h"

/***************** Main functions *************/
Radar::Radar() : pixels(NPIXELS, LEDS_PIN, NEO_GRB + NEO_KHZ800) {
}


void Radar::initialize(bool displayAlwaysON_, 
                       bool compassEnabled_,
                       bool gpsEnabled_,
                       bool showUnreachable_) {

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    // TODO: attachInterrupt doesn't work
    // attachInterrupt(2, (void (*)())(&onButtonPress), 1);
    // attachInterrupt(2, Radar::onButtonRelease, 1);

    displayAlwaysON = displayAlwaysON_;
    compassEnabled = compassEnabled_;
    gpsEnabled = gpsEnabled_;
    showUnreachable = showUnreachable_;

    serialBT.begin("DragonBallRadar");
    delay(5000);
    serialBT << "Connection established." << endl;

    // Pixels initialization
    serialBT << "Initializing pixels... ";
    pixels.begin();
    serialBT << "done." << endl;
    
    // Compass initialization
    if (compassEnabled) {
        serialBT << "Initializing compass... " << endl;
        compassInit();
        serialBT << "done." << endl;
    }

    // Targets initialization
    serialBT << "Initializing targets... ";
    EEPROM.begin(2 * NTARGETS * sizeof(double));
    loadTargets();
    serialBT << "done." << endl;

    // GPS initialization
    if (gpsEnabled) {
        serialBT << "Initializing GPS... ";
        gpsInit();
        serialBT << "done." << endl;
    }
}

void Radar::update(bool debug) {
    if (!displayAlwaysON && !displayON) {
        return;
    }
    if ((buttonPressed) && (millis() - last_press_ms > LONGPRESS_MS)) {
        // We have pressed the button for longer than LONGPRESS_MS
        bluetoothCommunication();
    }
    if ((!displayAlwaysON) &&  (millis() - last_press_ms > DISPLAY_MS)) {
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
    //delay(100);
}

esp_err_t do_firmware_upgrade() {
    esp_http_client_config_t config = {
        .url = CONFIG_FIRMWARE_UPGRADE_URL,
        .cert_pem = (char *)server_cert_pem_start,
    };
    esp_err_t ret = esp_https_ota(&config);
    if (ret == ESP_OK) {
        esp_restart();
    } else {
        return ESP_FAIL;
    }
    return ESP_OK;
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
    // serialBT << "Led 0: " << targets[0].ledId << endl;
}

/********** Battery Management ***************/
void Radar::readBatteryLevel() {
    int batLevel = analogRead(BATLEV_PIN);
}

/********** Compass functions ***************/
void Radar::compassInit() {
    compass.init();
    
    // get the compass calibration values from the EEPROM
    int xMin, xMax, yMin, yMax, zMin, zMax;
    EEPROM.get(COMPASS_ADDR_OFFSET, xMin);
    EEPROM.get(COMPASS_ADDR_OFFSET + sizeof(int), xMax);
    EEPROM.get(COMPASS_ADDR_OFFSET + 2*sizeof(int), yMin);
    EEPROM.get(COMPASS_ADDR_OFFSET + 3*sizeof(int), yMax);
    EEPROM.get(COMPASS_ADDR_OFFSET + 4*sizeof(int), zMin);
    EEPROM.get(COMPASS_ADDR_OFFSET + 5*sizeof(int), zMax);
    
    compass.setCalibration(xMin, xMax, yMin, yMax, zMin, zMax);

    // Check the compass, recalibrate if needed
    while(!compassCheck()) {
        serialBT << "Compass check failed." << endl;
        calibrateCompass();
    }
}

bool Radar::compassCheck() {
    // Very easy check of the compass range
    // User moves the compass and we classify the returned Azimuth in one bin
    // If all bins are filled the compass passes the check
    // If 1+ bin is never filled the function will timeout and the check will fail
    bool bins[36];
    bool done = false;
    int angle;
    int bin;
    unsigned long startTime = millis();
    int timeout_ms = 30000;

    serialBT << "Checking compass calibration." << endl;
    serialBT << "Please turn the compass in circles..." << endl;

    while(!done && (millis() - startTime < timeout_ms)) { 
        compass.read();
        angle = compass.getAzimuth();
        if (angle < 0) {
            serialBT << "Error, compass angle is " << angle << endl;
            angle = 0;
        } else if (angle >= 360) {
            serialBT << "Error, compass angle is " << angle << endl;
            angle = 359;
        }

        bin = angle / 10;
        bins[bin] = true;
        
        done = true;
        for (int i=0; i<36; i++) {
            if (!bins[i]) {
                done = false;
                break;
            }
        }
    }
    return done;
}

void Radar::calibrateCompass() {
    int calibrationData[3][2];
    bool changed = false;
    bool done = false;
    int t = 0;
    int c = 0;
    int x, y, z;
    
    serialBT << "Calibrating... Keep moving your sensor around." << endl;
    while(!done) {    
        // Read compass values
        compass.read();

        // Return XYZ readings
        x = compass.getX();
        y = compass.getY();
        z = compass.getZ();

        changed = false;
        if(x < calibrationData[0][0]) {
            calibrationData[0][0] = x;
            changed = true;
        }
        if(x > calibrationData[0][1]) {
            calibrationData[0][1] = x;
            changed = true;
        }
        if(y < calibrationData[1][0]) {
            calibrationData[1][0] = y;
            changed = true;
        }
        if(y > calibrationData[1][1]) {
            calibrationData[1][1] = y;
            changed = true;
        }
        if(z < calibrationData[2][0]) {
            calibrationData[2][0] = z;
            changed = true;
        }
        if(z > calibrationData[2][1]) {
            calibrationData[2][1] = z;
            changed = true;
        }

        if (changed && !done) {
            serialBT << "Calibrating... Keep moving your sensor around." << endl;
            c = millis();
        }
        t = millis();
        
        if ( (t - c > 5000) && !done) {
            done = true;
        }
    }
    serialBT << "Calibration finished:" << endl;
    serialBT << "  " << calibrationData[0][0] << ", " << calibrationData[0][1] << endl;
    serialBT << "  " << calibrationData[1][0] << ", " << calibrationData[1][1] << endl;
    serialBT << "  " << calibrationData[2][0] << ", " << calibrationData[2][1] << endl;
    
    compass.setCalibration(
        calibrationData[0][0], calibrationData[0][1],
        calibrationData[1][0], calibrationData[1][1],
        calibrationData[2][0], calibrationData[2][1]);

    // save calibration parameters to EEPROM
    EEPROM.get(COMPASS_ADDR_OFFSET, calibrationData[0][0]);
    EEPROM.get(COMPASS_ADDR_OFFSET + sizeof(int), calibrationData[0][1]);
    EEPROM.get(COMPASS_ADDR_OFFSET + 2*sizeof(int), calibrationData[1][0]);
    EEPROM.get(COMPASS_ADDR_OFFSET + 3*sizeof(int), calibrationData[1][1]);
    EEPROM.get(COMPASS_ADDR_OFFSET + 4*sizeof(int), calibrationData[2][0]);
    EEPROM.get(COMPASS_ADDR_OFFSET + 5*sizeof(int), calibrationData[2][1]);
    EEPROM.commit();
}

/*********** LED functions ****************/
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

void Radar::blinkLed() {  
    if (millis() - last_blink_ms > BLINK_MS) {
        if (ledStatus) {
            for(int i=0; i<NPIXELS; i++) {
                pixels.setPixelColor(i, pixels.Color(0, 0, 0));
                delay(1);
            }
            pixels.show();
            delay(10);
            ledStatus = false;
        } else {
            for (int i=0; i<NTARGETS; i++) {
                if (showUnreachable && (targets[i].distance >= RING_MAX_DIST[0])) {
                    pixels.setPixelColor(targets[i].ledId, pixels.Color(40, 0, 0));  // red
                    delay(10);
                } else {
                    pixels.setPixelColor(targets[i].ledId, pixels.Color(0, 40, 0));  // green
                    delay(10);
                }
            }
            pixels.show();
            ledStatus = true;
        }
        last_blink_ms = millis();
    }
}

void Radar::successLeds() {
    for (int i=0; i<5; i++) {
        for(int i=0; i<NPIXELS; i++) {
            pixels.setPixelColor(i, pixels.Color(0, 0, 0));
            delay(1);
        }
        pixels.show();
        delay(200);
        for(int i=0; i<NPIXELS; i++) {
            pixels.setPixelColor(i, C_GREEN);
            delay(1);
        }
        pixels.show();
        delay(200);
    }
}

void Radar::failureLeds() {
    for (int i=0; i<5; i++) {
        for(int i=0; i<NPIXELS; i++) {
            pixels.setPixelColor(i, pixels.Color(0, 0, 0));
            delay(1);
        }
        pixels.show();
        delay(200);
        for(int i=0; i<NPIXELS; i++) {
            pixels.setPixelColor(i, C_RED);
            delay(1);
        }
        pixels.show();
        delay(200);
    }
}

void Radar::waveLedSeq(int ringId, uint32_t ledColor) {
    // set all pixels in the current ring
    for(int i=0; i<NPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
        delay(1);
    }
    pixels.show();
    for (int i=OFFSET_RINGS[ringId]; i<OFFSET_RINGS[ringId]+RINGS[ringId]; i++) {
        pixels.setPixelColor(i, ledColor);
        delay(1);
    }
    pixels.show();
}

/*********** GPS functions ****************/
bool Radar::gpsInit() {
    Serial.begin(9600);
    while (!Serial);
    bool gpsReady = waitGpsSignal();
    if (!gpsReady) {
        // GPS timed out.
        // Check if:
        // Is anything being received from the GPS?
        // How much satellites does the GPS see?
        return false;
    }
    return true;
}

bool Radar::waitGpsSignal() {
    // We wait until the GPS returns a valid signal
    bool gpsFound = false;
    unsigned long start_gps_ms = millis();
    unsigned long last_led_change_ms = millis();
    int ringId = NRINGS-1;

    while(!gpsFound) {
        // Timeout
        if (millis() - start_gps_ms > GPS_TIMEOUT_MS) {
            serialBT << "Timed out" << endl;
            delay(100);
            failureLeds();
            return false;
        }
        // led wave figure
        if (millis() - last_led_change_ms > 500) {
            waveLedSeq(ringId--, C_YELLOW);
            if (ringId < 0)
                ringId = NRINGS-1;
            last_led_change_ms = millis();
        }
        gpsFound = updateGpsLocation();
        delay(1000);
    }
    successLeds();
    serialBT << "MyPosition: " << MyPosition[0] << ", " << MyPosition[1] << endl;
    return true;
}

bool Radar::updateGpsLocation() {
    // read gps and update the position
    
    while (Serial.available() > 0) {
        gps.encode(Serial.read());
        yield();  // not sure if yield here will make it not work
    }
    serialBT << "LAT: " << gps.location.lat() << endl;
    serialBT << "LON: " << gps.location.lng() << endl;
    serialBT << "SAT: " << gps.satellites.value() << endl;
    if (gps.location.isUpdated()) {
        MyPosition[0] = gps.location.lat();
        MyPosition[1] = gps.location.lng();
        serialBT << "My Position: " << MyPosition[0] << ", " << MyPosition[1] << endl; 

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
        return true;
    }
    return false;
}

/*********** BT functions ****************/
void Radar::bluetoothCommunication() {  
    // 1. initialize the bluetooth port
    mode = M_BLUETOOTH;
    serialBT.begin("DragonBallRadar");
    unsigned long last_bluetooth_ms = millis();
    
    int ringId = NRINGS-1;
    unsigned long last_led_change_ms = millis();
    String rx = "";

    // we need to receive "pong" as a handshake
    while(rx != "pong") {
        // Timeout
        if (millis() - last_bluetooth_ms > BT_TIMEOUT_MS) {
            serialBT << "Timed out" << endl;
            delay(100);
            serialBT.end();
            for(int i=0; i<NPIXELS; i++) {
                pixels.setPixelColor(i, pixels.Color(0, 0, 0));
                delay(1);
            }
            pixels.show();
            mode = M_NORMAL;
            return;
        }
        // led wave figure
        if (millis() - last_led_change_ms > 500) {
            serialBT << "ping" << endl;
            waveLedSeq(ringId--, C_BLUE);
            if (ringId < 0)
                ringId = NRINGS-1;
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
    for(int i=0; i<NPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
        delay(1);
    }
    pixels.show();
    mode = M_NORMAL;
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

/*********** Targets functions ****************/
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

/*********** Misc functions ****************/
void IRAM_ATTR Radar::onButtonPress() {
    buttonPressed = true;
    displayON = true;
    last_press_ms = millis();
}

void IRAM_ATTR Radar::onButtonRelease() {
    buttonPressed = false;
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

/***************** TEST FUNCTIONS **************/

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
    Serial.begin(9600);
    while (!Serial);
    while(1) {
        while (Serial.available() > 0) {
            gps.encode(Serial.read());
        }
        serialBT << "LAT: " << gps.location.lat() << endl;
        serialBT << "LON: " << gps.location.lng() << endl;
        serialBT << "SAT: " << gps.satellites.value() << endl;

        delay(800);
    }

    // String text = "";
    // char aa;
    // while(1) {
    //     bool bb = false;
    //     while (Serial.available() > 0) {
    //         aa = (char)Serial.read();
    //         text += aa;
    //         bb = gps.encode(aa);
    //         if (bb) {
    //             serialBT << text << endl;
    //             delay(100);
    //             text = "";
    //         }
    //     }
    //     //serialBT << text << endl << endl;
    //     delay(100);
    // }
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
