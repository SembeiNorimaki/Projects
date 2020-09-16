#include <TinyGPS++.h>
#include <Streaming.h>
#include <Adafruit_NeoPixel.h>
#include <QMC5883LCompass.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
//#include "config.h"

#ifndef D5
  #define D5 (14)
  #define D6 (12)
  #define D7 (13)
  #define D8 (15)
  #define TX (1)
#endif

//#define DEBUG 
#define GPS_RX     D5
#define GPS_TX     D6
#define LEDS_PIN   D7
#define BUTTON_PIN D8

#define GPS_BAUD 9600
#define FEED_NAME "gps"
#define NPIXELS 61
#define NRINGS 5
#define NTARGETS 7

typedef struct {
   double latitude;
   double longitude;
   unsigned long distance;
   double angle;
   double angleCorrected;
   int ledId;
} Target;

int RINGS[] =           {24,  16,  12,  8,   1};
int OFFSET_RINGS[] =     {0,  24,  40,  52, 60};
int RING_MAX_DIST[] = {15000, 10000, 5000, 2000, 500};

unsigned int last_blink_ms = 0;
bool ledStatus = false;
int compassAngle = 0;

double MyPosition[] = {47.383890, 8.510015};
Target targets[NTARGETS];

TinyGPSPlus gps;
SoftwareSerial SerialGPS;
QMC5883LCompass compass;
Adafruit_NeoPixel pixels(NPIXELS, LEDS_PIN, NEO_GRB + NEO_KHZ800);
//AdafruitIO_Feed *feed = io.feed(FEED_NAME);
DynamicJsonDocument doc(1024);


void init_target() {
  for(int i=0; i<NTARGETS; i++) {
    targets[i].latitude = 0; 
    targets[i].longitude = 0;
    targets[i].distance = 0;
    targets[i].angle = 0;
    targets[i].angleCorrected = 0;
    targets[i].ledId = 0;
  }
}

int distance2ring(int distance) {
  // target out of range returns outer ring
  if (distance > RING_MAX_DIST[0])  
    return 0; // outer ring

  for (int i=1; i<NRINGS; i++) {
    if (distance > RING_MAX_DIST[i]) {
      return i-1;
    }
  }
  // target closer than last RING_MAX_DIST value returns the most inner ring
  return NRINGS; 
}

void updateLedId(int targetId, int distance, int angle_) {
  // angle must be in the range [0..360)
  int ringId = distance2ring(distance);
  int nleds = RINGS[ringId];
  int ledIdAux = floor(0.5 + nleds * angle_ / 360);
  if (ledIdAux == nleds) {
    ledIdAux = 0;
  }
  targets[targetId].ledId = ledIdAux + OFFSET_RINGS[ringId];
  #ifdef DEBUG
//    Serial << "Ring Id: " << ringId << endl;
//    Serial << "Led Id Aux: " << ledIdAux << endl;
//    Serial << "Led Id: " << ledId << endl;
  #endif
}

void scan() {
  // read serial
  while (SerialGPS.available() > 0) {
    gps.encode(SerialGPS.read());
    yield();
  }
  if (gps.location.isUpdated()) {
    MyPosition[0] = gps.location.lat();
    MyPosition[1] = gps.location.lng();
      Serial << "Lat: " << _FLOAT(MyPosition[0], 6) 
             << " Lon: " << _FLOAT(MyPosition[1], 6) 
             << " Nsat: " << gps.satellites.value() << endl;

    // recalculate distances and angles to all targets
    for (int i=0; i<NTARGETS; i++) {
      targets[i].distance = (unsigned long)TinyGPSPlus::distanceBetween(
        MyPosition[0], MyPosition[1], targets[i].latitude, targets[i].longitude);
      targets[i].angle = TinyGPSPlus::courseTo(
        MyPosition[0], MyPosition[1], targets[i].latitude, targets[i].longitude);
    }
  }
  compass.read();
  compassAngle = compass.getAzimuth();

  for (int i=0; i<NTARGETS; i++) {
    targets[i].angleCorrected = targets[i].angle - compassAngle;
    if (targets[i].angleCorrected < 0)
      targets[i].angleCorrected += 360;  
    updateLedId(i, targets[i].distance, targets[i].angleCorrected);
  }
  
  #ifdef DEBUG
    Serial << "My position: " << MyPosition[0] << " " << MyPosition[1] << endl;
    Serial << "Azimuth: " << compassAngle << endl;
  #endif
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  init_target();
  Serial.begin(115200);  
  SerialGPS.begin(GPS_BAUD, SWSERIAL_8N1, GPS_RX, GPS_TX, false, 95, 11);

  compass.init();
  //compass.setCalibration(-3378, 1046, -1323, 1041, -436, 2893);

//  io.connect();
//  feed->onMessage(handleMessage);
//  while(io.status() < AIO_CONNECTED) {
//    Serial << ".";
//    delay(500);
//  }
//  Serial << " Connected." << endl;
//  feed->get();

  pixels.begin();
}

void loop() {
//  io.run();
//  if (digitalRead(BUTTON_PIN) == 0) {
//    elapsed_s = 0;
//    scan();
//  }

  if (millis() - last_blink_ms > 500) {
    if (ledStatus) {
      pixels.clear();
      pixels.show();
      ledStatus = false;
    } else {
      for (int i=0; i<NTARGETS; i++) {
        if (targets[i].distance >= MAXDIST) {
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
  scan();
}

//void handleMessage(AdafruitIO_Data *data) {
// TODO: Allow multiple Targets  
//  Serial << data->feedName() << ": " << data->value() << endl;
//  deserializeJson(doc, data->value());
//  JsonObject obj = doc.as<JsonObject>();
//  TargetPosition[0] = obj[String("latitude")];
//  TargetPosition[1] = obj[String("longitude")];
//  Serial << "MyPosition[0]: " << MyPosition[0];
//  Serial << "MyPosition[1]: " << MyPosition[1];
//}
