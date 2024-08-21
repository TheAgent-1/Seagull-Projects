#include <SoftwareSerial.h>
#include <TinyGPS++.h>

int RXPin = 2;
int TXPin = 3;
int GPSBaud = 9600;

TinyGPSPlus gps;
SoftwareSerial gpsSerial(RXPin, TXPin);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);
}

void loop() {
  // put your main code here, to run repeatedly:
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {

      if (gps.time.hour() < 10) Serial.print(F("0"));
        Serial.print(gps.time.hour());
        Serial.print(":");
        if (gps.time.minute() < 10) Serial.print(F("0"));
        Serial.print(gps.time.minute());
        Serial.print(":");
        if (gps.time.second() < 10) Serial.print(F("0"));
        Serial.print(gps.time.second());
        Serial.print(".");
        if (gps.time.centisecond() < 10) Serial.print(F("0"));
        Serial.println(gps.time.centisecond());

    }
  }
}
