// Board: XIAO_ESP32C3

//Libraries============================
#include <SoftwareSerial.h>  // Prerequisite 1
#include <SPI.h>             // Prerequisite 2

#include <TinyGPS++.h>       // GPS
#include <nRF24L01.h>        // NRF24L01 - 1
#include <RF24.h>            // NRF24L01 - 2
//Libraries=END========================

//Variables============================
//GPS
const int GPS_TX = 0;
const int GPS_RX = 0;
const int GPSBaud = 9600;

//NRF24L01
const int NRF_CE = 0;
const int NRF_CSN = 0;
const byte NRF_address[6] = "5EGU1";

//Variables=END========================

//Objects==============================
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);
TinyGPSPlus gps;

RF24 radio(NRF_CE, NRF_CSN);
//Objects=END==========================

//Main=Code============================
void setup() {
  Serial.begin(115200);
  GPS_Setup();
  NRF_Setup();
}

void loop() {

}
//Main=Code=END========================

//Secondary=Code=======================
void GPS_Setup() {
  gpsSerial.begin(GPSBaud);
  Serial.println("Started: GPS Communication");
}

void GPS_Loop() {
  //placeholder
}

void NRF_Setup() {
  radio.begin();
  radio.openWritingPipe(NRF_address);
  radio.stopListening();
  Serial.println("Started: NRF Radio");
}

void NRF_Loop() {
  //placeholder
}

//Secondary=Code=END===================
