#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  //Parameters: address, coloums, rows

void setup() {
  lcd.init();
  lcd.clear();
  lcd.backlight();

  lcd.print("Hello World!");
}

void loop() {
  
}