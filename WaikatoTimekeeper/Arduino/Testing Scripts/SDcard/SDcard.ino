#include <SdFat.h>

// Set the chip select pin for your SD card module
const int chipSelect = 7;  // Change this to match your wiring

SdFat sd;
File myFile;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  // Initialize SD card
  if (!sd.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // Open the file for writing
  myFile = sd.open("test.txt", FILE_WRITE);

  // Write to the file
  if (myFile) {
    Serial.println("Writing to test.txt...");
    myFile.println("Testing 1, 2, 3...");
    // Close the file
    myFile.close();
    Serial.println("Done.");
  } else {
    Serial.println("Error opening test.txt");
  }
}

void loop() {
  // Nothing here for this example
}
