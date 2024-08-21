// ===================================== LIBRARIES
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <SD.h>  // Include the SD library
// ===================================== END LIBRARIES

// ===================================== VARIABLES
// MAIN
int TimeZone = 12;

// RFID
#define RFID_RST_PIN 9
#define RFID_SS_PIN 10
String TagID = "";
int CardCount = 0;

int block = 2;
byte readbackblock[18];

// CARD LIST
const int MAX_CARDS = 50;
String cardList[MAX_CARDS];

// LCD
// No need for it to be here
// Moved to the LCD Object

// GPS
#define RX_PIN 2
#define TX_PIN 3
const int GPSBaud = 9600;

// SD
#define SD_CS 7  // Define the chip select pin for the SD card
// ===================================== END VARIABLES

// ===================================== OBJECTS
// RFID
MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN);
MFRC522::MIFARE_Key key;

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// GPS
TinyGPSPlus gps;
SoftwareSerial gpsSerial(RX_PIN, TX_PIN);

// ===================================== END OBJECTS

// ===================================== MAIN FUNCTIONS
void setup() {
  Serial.begin(9600);
  Serial.println();
  RFID_SETUP();
  LCD_SETUP();
  GPS_SETUP();
  SD_SETUP();  // Initialize SD card
  Serial.println(F("All Modules Are Ready.\nSTARTING."));
}

void loop() {
  LCD_LOOP(GPS_TIME(), 0, 0);  // Display the GPS time on the LCD
  GPS_TIME();
  if (RFID_LOOP()) {  // If there is a tag...
    lcd.setCursor(0, 1);
    lcd.print(TagID);
    writeDataToSD(TagID);  // Write the tag ID to SD card
  }
  LCD_LOOP(String(CardCount), 14, 0);
}

// ===================================== END MAIN FUNCTIONS

// ===================================== SECONDARY FUNCTIONS
// RFID
void RFID_SETUP() {
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println(F("RFID Ready."));
}

boolean RFID_LOOP() {
  // Getting ready for Reading PICCs
  if (!mfrc522.PICC_IsNewCardPresent()) {  //If a new PICC placed to RFID reader continue
    return false;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {  //Since a PICC placed get Serial and continue
    return false;
  }

  TagID = "";
  for (uint8_t i = 0; i < 4; i++) {                     // The MIFARE PICCs that we use have 4 byte UID
    TagID.concat(String(mfrc522.uid.uidByte[i], HEX));  // Adds the 4 bytes in a single String variable
  }
  Serial.println(TagID);


  if (isCardInList(TagID)) {
    Serial.println(F("Tag in list"));
    
    TagID = "";
    return false;
  }

  //Serial.println(String(TagID + " is not in the list, Adding..."));
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
  return timeString;
}

void ConvertToLocalTime(int& hour) {
  hour += TimeZone;
  if (hour >= 24) hour -= 24;
}

// SD
void SD_SETUP() {
  if (!SD.begin(SD_CS)) {
    Serial.println(F("SD card initialization failed!"));
    //while (1); // Loop indefinitely if SD card initialization fails
    return;
  }
  Serial.println(F("SD card initialization done."));
}

void writeDataToSD(String data) {
  // Enable the SD card by pulling the CS pin low
  //digitalWrite(SD_CS, LOW);

  bool existingFile = SD.exists("/data.txt");
  File dataFile;

  switch (existingFile) {
    case true:
      dataFile = SD.open("/data.txt", O_APPEND);  // Open "data.txt" file for writing
      if (dataFile) {
        dataFile.println(data);  // Write data to the file
        dataFile.close();        // Close the file
        Serial.println(F("Data written to SD card."));
      } else {
        Serial.println(F("Error opening data.txt for appending."));
      }
      break;

    case false:
      dataFile = SD.open("/data.txt", FILE_WRITE);  // Open "data.txt" file for writing
      if (dataFile) {
        dataFile.println(data);  // Write data to the file
        dataFile.close();        // Close the file
        Serial.println(F("Data written to SD card."));
      } else {
        Serial.println(F("Error opening data.txt for writing."));
      }
      break;
  }
  // Disable the SD card by pulling the CS pin high
  //digitalWrite(SD_CS, HIGH);
}
