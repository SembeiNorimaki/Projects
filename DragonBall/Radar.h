#ifndef Radar_h
#define Radar_h
#include "Arduino.h"
#include <TinyGPS++.h>
#include <Streaming.h>
#include <Adafruit_NeoPixel.h>
#include <QMC5883LCompass.h>
#include <ArduinoJson.h>
#include "BluetoothSerial.h"
#include <EEPROM.h>
#include "ArduinoJson.h"

#define BUTTON_PIN 12
#define LEDS_PIN   13
#define BATLEV_PIN 11

#define DEBUG 

#define GPS_BAUD 9600
#define FEED_NAME "gps"
#define NPIXELS 61
#define NRINGS 5
#define NTARGETS 1
#define BLINK_MS 500           // time on/off, period = BLINK_MS * 2
#define GPS_MS   300000        // update gps location every GPS_MS 
#define DISPLAY_MS 10000       // how long the leds blink
#define LONGPRESS_MS 5000      // Button Longpress duration to enter BT mode
#define BT_TIMEOUT_MS 30000    // BT communication timeout
#define GPS_TIMEOUT_MS 120000  // GPS timeout

#define C_RED pixels.Color(40, 0, 0)
#define C_YELLOW pixels.Color(40, 40, 0)
#define C_GREEN pixels.Color(0, 40, 0)
#define C_BLUE pixels.Color(0, 0, 40)

#define M_NORMAL 1
#define M_BLUETOOTH 2

// TODO: Define EEPROM memory locations
#define COMPASS_ADDR 20


typedef struct {
    double latitude;
    double longitude;
    unsigned long distance;
    double angle;
    double angleCorrected;
    int ledId;
} Target;

class Radar {
    public:
        Radar();
        void initialize();
        void update(bool debug);

        // test methods are public
        void test();
        void testLeds();
        void testGps();
        void testCompass();
        void testEeprom();
        void testParser();

        void storeTarget(int id, double lat, double lon);
        void setTarget(int id, double lat, double lon);
        void loadTargets();
        void sendTargets();
        void bluetoothCommunication();
        void successLeds();
        void failureLeds();
        void calibrateCompass();
        bool compassCheck();
        void compassInit();

    private:
        // Devices
        QMC5883LCompass compass;
        Adafruit_NeoPixel pixels;
        TinyGPSPlus gps;
        BluetoothSerial serialBT;
        
        // Methods
        void onButtonPress();
        void onButtonRelease();
        bool updateGpsLocation();
        void scan();
        void blinkLed();
        void updateLedId(int targetId, int distance, int angle_);
        int distance2ring(int distance);
        void decodeRX();
        void waveLedSeq(int ringId, uint32_t ledColor);
        void waitGpsSignal();
        void readBatteryLevel();


        
        // Variables
        int mode = M_NORMAL;
        unsigned long last_gps_ms = 0;
        unsigned long last_blink_ms = 0;
        unsigned long last_press_ms = 0;

        bool buttonPressed = false;
        bool displayON = false;
        bool displayAlwaysON = true;
        
        int compassAngle = 0;
        bool compassEnabled = false; // true: compass mode
                                    // false: cardinal mode
        
        bool showUnreachable = false; // true: unreachable targets shown in red
                                      // false: unreachable targets hidden

        bool ledStatus = false; 
        
        int RINGS[NRINGS] =       {24, 16, 12,  8,  1};
        int OFFSET_RINGS[NRINGS] = {0, 24, 40, 52, 60};
        int RING_MAX_DIST[NRINGS] = {90, 70, 50, 30, 10};
        Target targets[NTARGETS];
        double MyPosition[2] = {0, 0};

        int testId = 0;
};

#endif
