/*
Pinout:
Module   | Pin  | ESP32
-----------------------
GPS      | TX   | 35
GPS      | RX   | 21
Loadcell | TX   | 22
Loadcell | RX   | 27
All Modules Are 5V Compatable

Libraries:
TFT_eSPI - Bodmer
XPT2046_Touchscreen - Paul Stroffregen
TinyGPSPlus - Mikal Hart

Board:
ESP32 Dev Module
*/

//Libraries============================
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#include <SD.h>

#include "ExtraData.h"
//=====================================

//Global Variables=====================
// TFT Screen
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define FONT_SIZE 4

// GPS
const int GPS_RX = 35;
const int GPS_TX = 21;
const int GPS_Baud = 9600;
bool GPSFix = false;
bool PrintGPS = false;

// HX711
const int HX711_RX = 22;
const int HX711_TX = 27;
const int HX711_Baud = 9600;
bool PrintLoad = false;

// Non-Blocking delays
unsigned long previousMillis1 = 0UL;
unsigned long interval1 = 1000UL;     // 1 Second Delay

unsigned long previousMillis2 = 0UL;
unsigned long interval2 = 10000UL;    // 10 Second Delay
//=====================================

//Object Instances=====================
// TFT Screen
TFT_eSPI tft = TFT_eSPI();
SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

// GPS
TinyGPSPlus gps;
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);

// HX711
SoftwareSerial HX711Serial(HX711_RX, HX711_TX);

// SD
File myFile;

// Data Storage
ExtraData Data;
//=====================================

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");
  ScreenSetup(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS, 1);
  GPSSetup();
  HX711Setup();
  SDSetup();

}

void loop() {
  unsigned long currentMillis = millis();

  //Simple Command check
  if (Serial.available() > 0) {
    String IncomingData = Serial.readString();
    IncomingData.trim();
    if (IncomingData == "!PrintGPS") {
      PrintGPS = !PrintGPS;
    }
    if (IncomingData == "!PrintLoad") {
      PrintLoad = !PrintLoad;
    }
  }


  GPSLoop(PrintGPS);
  HX711Loop(PrintLoad);

  //Runs every 1 seconds
  if (currentMillis - previousMillis1 > interval1) {
    DisplayLoop();

    previousMillis1 = currentMillis;
  }


  //Runs every 10 seconds
  if (currentMillis - previousMillis2 > interval2) {
    SDLoop();
    previousMillis2 = currentMillis;
  }
}

//Secondary Functions==================
void ScreenSetup(int CLK, int MISO, int MOSI, int CS, int Rotation) {
  touchscreenSPI.begin(CLK, MISO, MOSI, CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(Rotation);

  tft.init();
  tft.setRotation(Rotation);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
}

void GPSSetup() {
  gpsSerial.begin(GPS_Baud);
}

void HX711Setup() {
  HX711Serial.begin(HX711_Baud);
}

void SDSetup() {

  int centerX = SCREEN_WIDTH / 2;
  int centerY = SCREEN_HEIGHT / 2;
  int oppX = SCREEN_WIDTH - 5;
  int oppY = SCREEN_HEIGHT - 20;
  if (!SD.begin()) {
    Serial.println("SD not starting");
    tft.drawCentreString("SD not starting", centerX, centerY, 4);
    while (true) {
      ;
      ;
    }
  }
  tft.drawRightString("SD Initilised", oppX, oppY, 2);
}

void ClearScreen(int Color) {
  tft.fillRect(0, 0, SCREEN_WIDTH - 10, SCREEN_HEIGHT - 20, Color);
  tft.fillRect(0, 200, 230, SCREEN_HEIGHT, Color);
}

void GPSLoop(bool print) {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      Data.Lat = gps.location.lat();
      Data.Long = gps.location.lng();
      Data.Speed = gps.speed.kmph();

      Data.GPS_Year = gps.date.year();
      Data.GPS_Month = gps.date.month();
      Data.GPS_Day = gps.date.day();
      Data.GPS_Hour = gps.time.hour();
      Data.GPS_Minute = gps.time.minute();
      Data.GPS_Second = gps.time.second();

      if (Data.Lat != 0 || Data.Long != 0) {
        GPSFix == true;
      }

      if (print) {
        Serial.print("Latitude: ");
        Serial.println(gps.location.lat(), 6);
        Serial.print("Longitude: ");
        Serial.println(gps.location.lng(), 6);
        Serial.print("Speed: ");
        Serial.println(gps.speed.kmph(), 2);
      }
    }
  }
}

void HX711Loop(bool print) {
   if (HX711Serial.available()) {
    Data.Force = HX711Serial.read();
   }

   if (print) {
    Serial.print("Force: ");
    Serial.println(Data.Force, 2);
   }
} 

void DisplayLoop() {
  int centerX = SCREEN_WIDTH / 2;
  int centerY = SCREEN_HEIGHT / 2;
  int oppX = SCREEN_WIDTH;
  int oppY = SCREEN_HEIGHT;

  ClearScreen(TFT_BLACK);

  tft.drawString("Speed: ", 10, 10, 4);
  tft.drawCentreString(String(Data.Speed), centerX, 70, 8);

  tft.drawString(Data.Time_string_HMS_edited(12), 10, 200, 4);

  if (GPSFix) {
    tft.drawRightString("GPS Fix", centerX, oppY - 40, 2);
  }
}

void SDLoop() {
 myFile = SD.open("/Data.csv", FILE_APPEND);
 String DataToWrite = Data.Time_string_HMS_edited(12-24) + "," + String(Data.Lat, 8) + "," + String(Data.Long, 8) + "," + String(Data.Speed, 2);
 if(myFile) {
  myFile.println(DataToWrite);
  myFile.close();
 }
}
