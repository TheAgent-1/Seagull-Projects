/*
Libraries:
TFT_eSPI - Bodmer
XPT2046_Touchscreen - Paul Stroffregen

Board:
ESP32 Dev Module
*/

//Libraries============================
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
//=====================================

//Global Variables=====================
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define FONT_SIZE 4
//=====================================

//Object Instances=====================
TFT_eSPI tft = TFT_eSPI();
SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);
//=====================================


void setup() {
  Serial.begin(115200);
  ScreenSetup(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS, 1);


  tft.drawString("Testing 123!@#", 10, 30, FONT_SIZE);
  delay(3000);
  ClearScreen(TFT_BLACK);
  
}

void loop() {
  // put your main code here, to run repeatedly:

}


void ScreenSetup(int CLK, int MISO, int MOSI, int CS, int Rotation) {
  touchscreenSPI.begin(CLK, MISO, MOSI, CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(Rotation);

  tft.init();
  tft.setRotation(Rotation);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
}

void ClearScreen(int Color) {
  tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color);
}