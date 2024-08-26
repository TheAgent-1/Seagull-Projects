// Board: XIAO_ESP32C3


//Libraries============================
#include <SoftwareSerial.h>       // Prerequisite 1
#include <SPI.h>                  // Prerequisite 2

#include <TinyGPS++.h>            // GPS
#include <LiquidCrystal_I2C.h>    // LCD I2C
#include <nRF24L01.h>             // NRF24L01 - 1
#include <RF24.h>                 // NRF24L01 - 2
//Libraries=END========================

//Variables============================
// Main
const bool Receiver = false;

// GPS
const int GPS_TX = 0;
const int GPS_RX = 0;
const int GPSBaud = 9600;

// LCD I2C
const int Cols = 16;
const int Rows = 2;

// NRF24L01
const int NRF_CE = 0;
const int NRF_CSN = 0;
const byte NRF_address[6] = "5EGU1";
char data[32] = "placeholder";
char buffer[32] = {0};

//Variables=END========================

//Objects==============================
// GPS
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);
TinyGPSPlus gps;

// LCD I2C
LiquidCrystal_I2C lcd(0x27, Cols, Rows);

// NRF24L01
RF24 radio(NRF_CE, NRF_CSN);
//Objects=END==========================

//Main=Code============================
void setup() {
  Serial.begin(115200);
  GPS_Setup();
  LCD_Setup();
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
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println("No GPS");
    while (true)
      ;
  }
}

void LCD_Setup() {
  lcd.init();
  lcd.clear();         
  lcd.backlight();
}

void LCD_Loop() {
  String LAT = String(gps.location.lat());
  String LONG = String(gps.location.lng());

  lcd.setCursor(0, 0);
  lcd.print(String("Lat: " + LAT));
  lcd.setCursor(0, 1);
  lcd.print(String("Lng: " + LONG));
}

void NRF_Setup() {
  radio.begin();

  if (!Receiver) {
    radio.openWritingPipe(NRF_address);
    radio.stopListening();
    Serial.println("Started: NRF Radio");
    Serial.println("NRF mode: Transmitter");
  } else {
    radio.openReadingPipe(0, NRF_address);
    radio.startListening();
    Serial.println("Started: NRF Radio");
    Serial.println("NRF mode: Receiver");
  }
}

void NRF_Loop() {
  //placeholder
}

//Secondary=Code=END===================
