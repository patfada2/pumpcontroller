#include <LiquidCrystal_I2C.h>
// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
boolean lcdIsLineZero;

void lcdNewLine() {
  if (lcdIsLineZero) {
    lcd.setCursor(0, 1);
    lcdIsLineZero = false;

  } else {
    lcd.setCursor(0, 0);
    lcdIsLineZero = true;
  }
}

void lcdWrite(String msg) {
  Serial.println(msg);
  //pad right to ensure full line is ovewritten
  while (msg.length() < 16) {
    msg += " ";
  }
  lcd.print(msg);
  lcdNewLine();
  
}

void lcdWriteln(String msg) {
  lcdWrite(msg);
  lcdNewLine();
}

void setupLCD() {

  lcd.init();
  // turn on LCD backlight
  lcd.backlight();
  lcd.noCursor();
  lcdIsLineZero = true;
  lcdWrite("PumpController");
}