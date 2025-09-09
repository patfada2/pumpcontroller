#include <Arduino.h>
#include <ArduinoJson.h>
#include "lcdutils.h"
#include <LiquidCrystal_I2C.h>
#include "common.h"


// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;
boolean lcdEnabled = false; // make this true if the LCD is wired up, otherwise it seems to mess up the setup/loop code

  // set LCD address, number of columns and rows
  // if you don't know your display address, run an I2C scanner sketch
  LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
boolean lcdIsLineZero;

void lcdNewLine() {
  if (lcdEnabled) {
    if (lcdIsLineZero) {
      lcd.setCursor(0, 1);
      lcdIsLineZero = false;

    } else {
      lcd.setCursor(0, 0);
      lcdIsLineZero = true;
    }
  }
}

void lcdWrite(String msg) {
  logInfo(msg);

  if (lcdEnabled) {
    //pad right to ensure full line is ovewritten
    while (msg.length() < 16) {
      msg += " ";
    }
    lcd.print(msg);
    lcdNewLine();
  }
}

void lcdWriteln(String msg) {
  if (lcdEnabled) {
    lcdWrite(msg);
    lcdNewLine();
  }
}


void lcdDisplayStatus(String msg1, String msg2) {
  logInfo(msg1);
  logInfo(msg2);

  if (lcdEnabled) {
    
    lcd.clear();
    //lcd.blink();
    lcd.setCursor(0, 0);
    lcdWrite(msg1);

    lcd.setCursor(0, 1);
    lcdWrite(msg2);

    lcdIsLineZero = false;
  }
}


void setupLCD() {

  if (lcdEnabled) {
    lcd.init();
    // turn on LCD backlight
    lcd.backlight();
    lcd.noCursor();
    lcdIsLineZero = true;
    lcdWrite("PumpController");
  }
}