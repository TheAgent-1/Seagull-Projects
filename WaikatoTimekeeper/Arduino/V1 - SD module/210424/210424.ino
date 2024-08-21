/*

"Now thats a spicy pillow" - Jacob Croul

*/

// TimeKeeper
// ===================================== LIBRARIES
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <SdFat.h>
// ===================================== END LIBRARIES

// ===================================== VARIABLES
// MAIN
int TimeZone = 12;

// RFID
const int RFID_RST = 9;
const int RFID_CS = 10;
String TagID = "";
int CardCount = 0;

// CARD LIST
const int MAX_CARDS = 50;
String cardList[MAX_CARDS];

// LCD
const int LCD_COL = 16;
const int LCD_ROW = 2;

// GPS
const int RX_PIN = 2;
const int TX_PIN = 3;
const int GPSBaud = 9600;

// SD
const int SD_CS = 7;
// ===================================== END VARIABLES

// ===================================== OBJECTS
// RFID
MFRC522 mfrc522(RFID_CS, RFID_RST);
MFRC522::MIFARE_Key key;

// LCD
LiquidCrystal_I2C lcd(0x27, LCD_COL, LCD_ROW);

// GPS
TinyGPSPlus gps;
SoftwareSerial gpsSerial(RX_PIN, TX_PIN);

// SD
SdFat sd;
File myFile;
// ===================================== END OBJECTS

// ===================================== MAIN FUNCTIONS
void setup() {
  Serial.begin(9600);
  Serial.println();
  RFID_SETUP();
  LCD_SETUP();
  GPS_SETUP();
  Serial.println(F("All Modules Are Ready.\nSTARTING."));
}

void loop() {
  LCD_LOOP(GPS_TIME(), 0, 0); // Display the GPS time on the LCD
  //GPS_TIME();
  if (RFID_LOOP()) {  // If there is a tag...
    LCD_LOOP(TagID, 0, 1);  // Display the tag id
    LCD_LOOP(String(CardCount), 14, 0);
  }
  if (gps.location.isValid()) {
    lcd.setCursor(15, 1);
    lcd.write(255);
  } else {
    lcd.setCursor(15, 1);
    lcd.print(" ");
  }
 
}
// ===================================== END MAIN FUNCTIONS

// ===================================== SECONDARY FUNCTIONS
// RFID
void RFID_SETUP() {
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println(F("RFID Ready."));
}

bool RFID_LOOP() {
  // Getting ready for Reading PICCs
  if (!mfrc522.PICC_IsNewCardPresent()) {  //If a new PICC placed to RFID reader continue
    return false;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {  //Since a PICC placed get Serial and continue
    return false;
  }

  TagID = "";
  for (uint8_t i = 0; i < 4; i++) {  // The MIFARE PICCs that we use have 4 byte UID
    TagID.concat(String(mfrc522.uid.uidByte[i], HEX));  // Adds the 4 bytes in a single String variable 
  }

  if (isCardInList(TagID)) {
    TagID = "";
    return false;
  }

  addCardToList(TagID);
  CardCount++;

  TagID.toUpperCase();
  mfrc522.PICC_HaltA();  // Stop reading
  
  return true;
}

// CARD LIST
void addCardToList(String id) {
  if (CardCount < MAX_CARDS) {
    cardList[CardCount] = id;
  }
}

bool isCardInList(String id) {
  for (int i = 0; i < CardCount; i++) {
    if (cardList[i] == id) {
      return true;
    }
  }
  return false;
}

// LCD
void LCD_SETUP() {
  lcd.init();
  lcd.clear();
  lcd.backlight();
  Serial.println(F("LCD Ready."));
}

void LCD_LOOP(String message, int col, int row) {
  lcd.setCursor(col, row);

  lcd.print(message);
}

// GPS
void GPS_SETUP() {
  gpsSerial.begin(GPSBaud);
  Serial.println(F("GPS Ready."));
}

String GPS_TIME() {
  String timeString = "";
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {

      int utc_hour = gps.time.hour();
      ConvertToLocalTime(utc_hour);  // Convert the current gps time (UTC) to the correct timezone

      // Save time to string
      if (utc_hour < 10) timeString += "0";
      timeString += String(utc_hour) + ":";
      if (gps.time.minute() < 10) timeString += "0";
      timeString += String(gps.time.minute()) + ":";
      if (gps.time.second() < 10) timeString += "0";
      timeString += String(gps.time.second());
      break;
    }
  }
  Serial.println(F("Clock"));
  return timeString;
}

void ConvertToLocalTime(int& hour) {
  hour += TimeZone;
  if (hour >= 24) hour -= 24;
}
// ===================================== END SECONDARY FUNCTIONS