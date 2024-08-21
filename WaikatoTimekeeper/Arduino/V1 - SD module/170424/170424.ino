// ===================================== LIBRARIES
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <SD.h> // Include the SD library
// ===================================== END LIBRARIES

// ===================================== VARIABLES
// MAIN
int TimeZone = 12;

// RFID
const int RFID_RST_PIN = 9;
const int RFID_SS_PIN = 10;
String TagID = "";
String LastTagID = "";
int CardCount = 0;

int block=2;
byte readbackblock[18];

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
const int SD_CS = 7; // Define the chip select pin for the SD card
// ===================================== END VARIABLES

// ===================================== OBJECTS
// RFID
MFRC522 mfrc522(RFID_SS_PIN, RFID_RST_PIN);
MFRC522::MIFARE_Key key;

// LCD
LiquidCrystal_I2C lcd(0x27, LCD_COL, LCD_ROW);

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
  //SD_SETUP(); // Initialize SD card
  Serial.println("All Modules Are Ready.\nSTARTING.");
}

void loop() {
  LCD_LOOP(GPS_TIME(), 0, 0); // Display the GPS time on the LCD
  GPS_TIME();
  if (RFID_LOOP()) {  // If there is a tag...
    lcd.setCursor(0, 1);
    for(int j=0; j<7; j++) {
      lcd.write(readbackblock[j]);
      Serial.write(readbackblock[j]);
    }
    LCD_LOOP(String(CardCount), 14, 0);
    //writeDataToSD(TagID); // Write the tag ID to SD card
  }
}
// ===================================== END MAIN FUNCTIONS

// ===================================== SECONDARY FUNCTIONS
// RFID
void RFID_SETUP() {
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("RFID Ready.");
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
    Serial.println("Hello World!");

    Serial.println(TagID + "is already in the list");
    TagID = "";
    return false;
  }

  //Serial.println(String(TagID + " is not in the list, Adding..."));
  addCardToList(TagID);
  CardCount++;

  readBlock(block, readbackblock);

  TagID.toUpperCase();
  mfrc522.PICC_HaltA();  // Stop reading
  
  return true;
}

int readBlock(int blockNumber, byte arrayAddress[]) {
  int largestModulo4Number = blockNumber / 4 * 4;
  int trailerBlock = largestModulo4Number + 3;  //determine trailer block for the sector

  //authentication of the desired block for access
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed (read): ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 3;  //return "3" as error message
  }

  //reading a block
  byte buffersize = 18;                                                  //we need to define a variable with the read buffer size, since the MIFARE_Read method below needs a pointer to the variable that contains the size...
  status = mfrc522.MIFARE_Read(blockNumber, arrayAddress, &buffersize);  //&buffersize is a pointer to the buffersize variable; MIFARE_Read requires a pointer instead of just a number
  if (status != MFRC522::STATUS_OK) {
    Serial.print("MIFARE_read() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 4;  //return "4" as error message
  }
  Serial.println("block was read");
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
  Serial.println("LCD Ready.");
}

void LCD_LOOP(String message, int col, int row) {
  lcd.setCursor(col, row);
  lcd.print(message);
}

// GPS
void GPS_SETUP() {
  gpsSerial.begin(GPSBaud);
  Serial.println("GPS Ready.");
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
    Serial.println("SD card initialization failed!");
    //while (1); // Loop indefinitely if SD card initialization fails
    return;
  }
  Serial.println("SD card initialization done.");
}

void writeDataToSD(String data) {
  // Enable the SD card by pulling the CS pin low
  digitalWrite(SD_CS, LOW);

  bool existingFile = SD.exists("/data.txt");
  File dataFile;

  switch (existingFile) {
    case true:
      dataFile = SD.open("/data.txt", O_APPEND);  // Open "data.txt" file for writing
      if (dataFile) {
        dataFile.println(data);  // Write data to the file
        dataFile.close();        // Close the file
        Serial.println("Data written to SD card.");
      } else {
        Serial.println("Error opening data.txt for writing.");
      }
      break;

    case false:
      dataFile = SD.open("/data.txt", O_WRITE);  // Open "data.txt" file for writing
      if (dataFile) {
        dataFile.println(data);  // Write data to the file
        dataFile.close();        // Close the file
        Serial.println("Data written to SD card.");
      } else {
        Serial.println("Error opening data.txt for writing.");
      }
      break;
  }
  // Disable the SD card by pulling the CS pin high
  digitalWrite(SD_CS, HIGH);
}
