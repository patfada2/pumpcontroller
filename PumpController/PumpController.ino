#include <LittleFS.h>

/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-plot-chart-web-server/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  This code builds a web server which connetcs tot he lcoal wifi and a loop whcih reads the sensors.
  The loop reads the time -  this function calls a rest API - it doesnt seem to work from within the webserver callback code
  It also reads the  sensor data 
  The latest time and sensor data values read by the loop  are stored in globals which are read by the web application defined in index html
  which calls  /GET_MOISTURE on the web server every 30 seconds.

*********/
#ifdef ESP32
#include <LittleFS.h>
#include <FS.h>
#else
#include <Arduino.h>
#include <Hash.h>
#endif
#include <Wire.h>

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <LiquidCrystal_I2C.h>

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

// Replace with your network credentials
const char* ssid = "RosieWiFi";
const char* password = "Thr33.0n3";
const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0

const int relayPin = 4;  //d2
const double vcal = 44.0;
boolean relayIsOn;

//29 June 2024
time_t dateTime = 1719685735000;  //29 June 2024

double A0toV(int a0) {
  return a0 / vcal;
}

int readA0() {

  // read the analog in value
  int a = analogRead(analogInPin);
  if (isnan(a)) {
    Serial.println("Failed to read from A0 sensor!");
    return 0;
  } else {
    Serial.print("sensor A0 = ");
    Serial.println(a);
    return a;
  }
}

void relayOn() {
  digitalWrite(relayPin, LOW);
  digitalWrite(LED_BUILTIN, HIGH);
  relayIsOn = true;
}

void relayOff() {
  digitalWrite(relayPin, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
  relayIsOn = false;
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


//a0: 0V = 0
//VCC =1024 =3.2

void setup() {

  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();
  lcdIsLineZero = true;


  // Serial port for debugging purposes
  Serial.begin(115200);
  delay(10);
  lcdWrite("PumpController");
  pinMode(LED_BUILTIN, OUTPUT);
  //d1 = gpo5
  pinMode(relayPin, OUTPUT);
  //relayOff();
}



const int interval = 2;  //sampling ingerval in seconds
const double vOn = 13.3;
const double vOff = 11.0;
const int maxSecondsOnPerDay = 4800;
const int secondsInDay = 3600 * 24;
int secondsElapsed = 0;
int secondsOn = 0;

double vin;

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(interval * 1000);
  digitalWrite(LED_BUILTIN, LOW);

  lcdWrite("checking..");
  vin = A0toV(readA0());
  Serial.println("seconds elapsed = " + String(secondsElapsed) + ", secondsOn=" + String(secondsOn) + ", maxSecondsOnPerDay =" + String(maxSecondsOnPerDay) + ",  vin = " + String(vin, 2));

  secondsElapsed += interval;
  if (secondsElapsed > secondsInDay) {
    secondsElapsed = 0;
  }
  if (relayIsOn) {
    secondsOn += interval;
    lcdWrite("ON v=" + String(vin, 2));
  } else lcdWrite("OFF v=" + String(vin, 2));

  lcdWrite(String(secondsOn) + "/" + String(maxSecondsOnPerDay));

  if ((secondsElapsed < maxSecondsOnPerDay) and (relayIsOn == false) and (vin > vOn)) {
    Serial.println("turning relay on");
    relayOn();
  }

  if (relayIsOn and ((vin < vOff) or (secondsOn > maxSecondsOnPerDay))) {
    Serial.println("turning relay off");
    relayOff();
  }
}
