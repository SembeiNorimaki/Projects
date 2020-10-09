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

#define DEBUG 
#define GPS_BAUD 9600
#define FEED_NAME "gps"
#define NPIXELS 61
#define NRINGS 5
//#define NTARGETS 3
#define BLINK_MS 500      // time on/off, period = BLINK_MS * 2
#define GPS_MS   300000   // update gps location every GPS_MS 
#define DISPLAY_MS 10000
#define LONGPRESS_MS 5000
#define BT_TIMEOUT_MS 30000

#define M_NORMAL 1
#define M_BLUETOOTH 2


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

    private:
        // Devices
        QMC5883LCompass compass;
        Adafruit_NeoPixel pixels;
        TinyGPSPlus gps;
        BluetoothSerial serialBT;
        
        // Methods
        void onButtonPress();
        void onButtonRelease();
        void updateGpsLocation();
        void scan();
        void blinkLed();
        void updateLedId(int targetId, int distance, int angle_);
        int distance2ring(int distance);
        void decodeRX();
        void btLedSeq(int ringId);
        
        
        // Variables
        int mode = M_NORMAL;
        unsigned int last_gps_ms = 0;
        unsigned int last_blink_ms = 0;
        unsigned int last_press_ms = 0;

        bool buttonPressed = false;
        bool displayON = false;
        
        int compassAngle = 0;
        bool compassEnabled = true; // true: compass mode
                                    // false: cardinal mode
        
        bool showUnreachable = false; // true: unreachable targets shown in red
                                      // false: unreachable targets hidden

        bool ledStatus = false; 
        int NTARGETS = 3;
        
        int RINGS[NRINGS] =       {24, 16, 12,  8,  1};
        int OFFSET_RINGS[NRINGS] = {0, 24, 40, 52, 60};
        int RING_MAX_DIST[NRINGS] = {90, 70, 50, 30, 10};
        Target targets[3];   //TODO: Hack. Fix it
        double MyPosition[2] = {47.383890, 8.510015};

        int testId = 0;
};

#endif
