/*
Pinout:
Module | Pin  | ESP32
---------------------
GPS    | TX   | 35
GPS    | RX   | 22
All Modules Are 5V Compatable

Libraries:
TFT_eSPI - Bodmer
XPT2046_Touchscreen - Paul Stroffregen

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
//TFT Screen
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define FONT_SIZE 4

//GPS
int RXPin = 35;
int TXPin = 22;
int GPSBaud = 9600;
//=====================================

//Object Instances=====================
//TFT Screen
TFT_eSPI tft = TFT_eSPI();
SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

//GPS
TinyGPSPlus gps;
SoftwareSerial gpsSerial(RXPin, TXPin);

//SD
File myFile;

//Data Storage
ExtraData Data;
//=====================================


void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");
  ScreenSetup(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS, 1);
  GPSSetup();
  SDSetup();
}

void loop() {
  GPSLoop();
  delay(1000);
  

}


void ScreenSetup(int CLK, int MISO, int MOSI, int CS, int Rotation) {
  touchscreenSPI.begin(CLK, MISO, MOSI, CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(Rotation);

  tft.init();
  tft.setRotation(Rotation);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
}

void ClearScreen(int Color) {
  tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color);
}

void GPSSetup() {
  gpsSerial.begin(GPSBaud);
}

void GPSLoop() {
  Serial.println("Hello");
  int oppX = SCREEN_WIDTH;
  int oppY = SCREEN_HEIGHT;
  while (gpsSerial.available() > 0) {

    Serial.println("W");

    if (gps.encode(gpsSerial.read())) {

      

      if (gps.location.isValid()) {

        tft.drawRightString("GPS Fix", oppX - 5, oppY - 40, 2);

        Data.Lat = gps.location.lat();
        Data.Long = gps.location.lng();
        Data.Speed = gps.speed.kmph();

        
        tft.fillRect(50, 20, 50, 50, TFT_BLACK);
        tft.drawString("Speed:", 10, 10, 2);
        tft.drawString(String(Data.Speed), 50, 20, 2);
        

        

        Serial.print("Latitude: ");
        Serial.println(gps.location.lat(), 6);
        Serial.print("Longitude: ");
        Serial.println(gps.location.lng(), 6);
        Serial.print("Speed: ");
        Serial.println(gps.speed.kmph(), 2);
      } else if (!gps.location.isValid() || gps.location.age() > 1500){
        tft.drawRect(oppX - 60, oppY - 40, 55, 10, TFT_BLACK);
      }
    }
  }
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
      ;;
    }
  }
  tft.drawRightString("SD Initilised", oppX, oppY, 2);
}