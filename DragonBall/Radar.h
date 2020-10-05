#ifndef Radar_h
#define Radar_h
#include "Arduino.h"
#include <TinyGPS++.h>
#include <Streaming.h>
#include <Adafruit_NeoPixel.h>
#include <QMC5883LCompass.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

#define DEVICE_ARDUINO
// #define DEVICE_ESP32
// #define DEVICE_ESP01

#ifdef DEVICE_ARDUINO
  #define GPS_RX     4
  #define GPS_TX     5
  #define BUTTON_PIN 8
  #define LEDS_PIN   3
#endif
#ifdef DEVICE_ESP32
  #define GPS_RX     D5
  #define GPS_TX     D6
  #define BUTTON_PIN D8
  #define LEDS_PIN   D7
#endif
#ifdef DEVICE_ESP01
  #define GPS_RX     0
  #define GPS_TX     2
  #define BUTTON_PIN 0
  #define LEDS_PIN   2
#endif

#define DEBUG 
#define GPS_BAUD 9600
#define FEED_NAME "gps"
#define NPIXELS 61
#define NRINGS 5
#define NTARGETS 1
#define BLINK_MS 500  // time on/off, period = BLINK_MS * 2
#define GPS_MS   300000 // update gps location every GPS_MS 

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
        void testLeds();
        void testGps();
        void testCompass();
    private:
        // devices
        QMC5883LCompass compass;
        Adafruit_NeoPixel pixels; //(NPIXELS, LEDS_PIN, NEO_GRB + NEO_KHZ800);
        TinyGPSPlus gps;
        SoftwareSerial serialGPS;
        
        // #ifdef DEVICE_ESP32 || DEVICE_ESP01
        //   SoftwareSerial serialGPS;
        // #endif
        // #ifdef DEVICE_ARDUINO
        //   SoftwareSerial serialGPS;
        // #endif
        
        // methods
        void initializeTargets();
        void setTarget(int id, double lat, double lon);
        void updateGpsLocation();
        void scan();
        void blinkLed();
        void updateLedId(int targetId, int distance, int angle_);
        int distance2ring(int distance);
        
        // variables
        unsigned int last_gps_ms = 0;
        unsigned int last_blink_ms = 0;
        int compassAngle = 0;
        bool compassEnabled = true; // true: compass mode
                                    // false: cardinal mode
        bool showUnreachable = false; // true: unreachable targets shown in red
                                      // false: unreachable targets hidden

        bool ledStatus = false;        
        int RINGS[NRINGS] =       {24, 16, 12,  8,  1};
        int OFFSET_RINGS[NRINGS] = {0, 24, 40, 52, 60};
        int RING_MAX_DIST[NRINGS] = {500, 400, 300, 200, 100};
        Target targets[NTARGETS]; 
        double MyPosition[2] = {47.383890, 8.510015};
};

#endif
