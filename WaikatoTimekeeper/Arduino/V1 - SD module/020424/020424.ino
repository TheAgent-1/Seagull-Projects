#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#define RST_PIN 9
#define SS_PIN 10

int RXPin = 2;
int TXPin = 3;
int GPSBaud = 9600;

byte readCard[4];
String MasterTag = "1911ED6E";  // REPLACE this Tag ID with your Tag ID!!!
String tagID = "";

int counter = 0;

// Create instances
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);  //Parameters: (rs, enable, d4, d5, d6, d7)
TinyGPSPlus gps;
SoftwareSerial gpsSerial(RXPin, TXPin);

void setup() {
  // Initiating
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);
  SPI.begin();         // SPI bus
  mfrc522.PCD_Init();  // MFRC522
  lcd.init();
  lcd.clear();
  lcd.backlight();

  lcd.clear();
  lcd.print(" Access Control ");
  lcd.setCursor(0, 1);
  lcd.print("Scan Your Card>>");
}

void loop() {

  //Wait until new tag is available
  while (getRFID()) {
    lcd.clear();




    lcd.setCursor(0, 0);
    lcd.print(getGPSTime());

    lcd.setCursor(0, 1);
    lcd.print(tagID);

    lcd.setCursor(14, 0);
    lcd.print(counter);

    delay(1000);
  }
}

//Read new tag if available
boolean getRFID() {
  // Getting ready for Reading PICCs
  if (!mfrc522.PICC_IsNewCardPresent()) {  //If a new PICC placed to RFID reader continue
    return false;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {  //Since a PICC placed get Serial and continue
    return false;
  }
  tagID = "";
  for (uint8_t i = 0; i < 4; i++) {  // The MIFARE PICCs that we use have 4 byte UID
    //readCard[i] = mfrc522.uid.uidByte[i];
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX));  // Adds the 4 bytes in a single String variable
  }
  counter++;
  tagID.toUpperCase();
  mfrc522.PICC_HaltA();  // Stop reading

  return true;
}

String getGPSTime() {
  String time = "";
  String HOUR;
  String MINUTE;
  String SECOND;
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.time.isValid()) {


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

        HOUR = gps.time.hour();
        MINUTE = gps.time.minute();
        SECOND = gps.time.second();
        time = HOUR + ":" + MINUTE + ":" + SECOND;


      } else {
        Serial.println("No Time");
      }
    }
  }
  return time;
}

double getGPSLat() {
  if (gps.location.isValid()) {
    return gps.location.lat();
  }
}

double getGPSLong() {
  if (gps.location.isValid()) {
    return gps.location.lng();
  }
}