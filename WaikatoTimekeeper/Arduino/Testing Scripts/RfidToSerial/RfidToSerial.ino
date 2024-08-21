#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 13
#define SS_PIN 5

String tagID = "";

MFRC522 mfrc522(SS_PIN, RST_PIN);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SPI.begin();         // SPI bus
  mfrc522.PCD_Init();  // MFRC522
}

void loop() {
  // put your main code here, to run repeatedly:
  if (getRFID()) {
    Serial.println(tagID);
  }
}

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
  
  tagID.toUpperCase();
  mfrc522.PICC_HaltA();  // Stop reading

  return true;
}
