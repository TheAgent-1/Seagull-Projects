/*
Pinout:

Module | Pin  | ESP32
---------------------
GPS    | TX   | 17
GPS    | RX   | 16
---------------------
OLED   | SDA  | SDA
OLED   | SCL  | SCL
---------------------
SD     | MISO | 19
SD     | MOSI | 23
SD     | SCK  | 18
SD     | CS   | 5

All Modules Are 5V Compatable
*/

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#include <SPI.h>
#include <SD.h>

#include "ExtraData.h"

int RXPin = 17;
int TXPin = 16;
int GPSBaud = 9600;

#define SCREEN_WIDTH   128    // OLED display width, in pixels
#define SCREEN_HEIGHT  64     // OLED display height, in pixels
#define OLED_RESET     -1     // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C

const int chipSelect = 5;

unsigned long previousMillis1 = 0UL;
unsigned long interval1 = 2000UL;
unsigned long previousMillis2 = 0UL;
unsigned long interval2 = 10000UL;


TinyGPSPlus gps;
SoftwareSerial gpsSerial(RXPin, TXPin);

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

File myFile;

ExtraData Data;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  gpsSerial.begin(GPSBaud);
  display.begin(SCREEN_ADDRESS, true);
  display.display();
  display.clearDisplay();
  display.drawPixel(10, 10, SH110X_WHITE);
  display.display();

  if (!SD.begin()) {
    Serial.println(F("SD Issue"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
  DisplayGPSSpeed();

  //Runs every 2 seconds
  if(currentMillis - previousMillis1 > interval1) {
	DisplayGPSSpeed();

 	previousMillis1 = currentMillis;
  }

  
  //Runs every 10 seconds
  if(currentMillis - previousMillis2 > interval2) {
	SaveDataToSDCard();

 	previousMillis2 = currentMillis;
  }
}

void DisplayGPSSpeed() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {

        Data.Lat = gps.location.lat();
        Data.Long = gps.location.lng();
        Data.Speed = gps.speed.kmph();
        
        Serial.print("Latitude: ");
        Serial.println(gps.location.lat(), 9);
        Serial.print("Longitude: ");
        Serial.println(gps.location.lng(), 9);
        Serial.print("Speed: ");
        Serial.println(gps.speed.kmph(), 2);

        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SH110X_WHITE);
        display.setCursor(0,0);
        display.print("Speed:");

        display.setTextSize(3); // ITS RIGHT HERE, CHANGE THIS IF YOU WANT
        display.setCursor(0,28);
        display.print(gps.speed.kmph());
        display.display();

      }
    }
  }
}

void SaveDataToSDCard() {
  Serial.print("\n\n\nSaving Data\n\n\n");

  myFile = SD.open("/Data.csv", FILE_APPEND);
  if(myFile) {
    myFile.println(Data.SDData());
    myFile.close();
  }
}