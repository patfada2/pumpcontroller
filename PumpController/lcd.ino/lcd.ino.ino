/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <LiquidCrystal_I2C.h>

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

void setup(){
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
  // Serial port for debugging purposes
  Serial.begin(115200);
  delay(10);
 
  Serial.println("lcd setup complete");
}

void loop(){
  // set cursor to first column, first row
  lcd.blink_on();
  lcd.setCursor(0, 0);
  // print message
  lcd.print("Hello, World!");
  Serial.println(".");
  delay(1000);
  lcd.blink_off();
  // clears the display to print new message
  lcd.clear();
  // set cursor to first column, second row
  lcd.setCursor(0,1);
  lcd.print("Hello, World!");
  delay(1000);
  lcd.clear(); 
}

