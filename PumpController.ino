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
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <FS.h>
#else
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#endif
#include <Wire.h>

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "./timeutils.h"
#include "./fileutils.h"

// Replace with your network credentials
const char* ssid = "RosieWiFi";
const char* password = "Thr33.0n3";
const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0

const int relayPin = 4;    //d2
const double vcal = 44.0;  //d2
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

//a0: 0V = 0
//VCC =1024 =3.2

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  delay(10);
  Serial.println("Hello!!");
  pinMode(LED_BUILTIN, OUTPUT);
  //d1 = gpo5
  pinMode(relayPin, OUTPUT);
  relayOff();
}



const int interval = 30; //sampling ingerval in seconds
const double vOn = 14.1;
const double vOff = 11.0;
const int maxSecondsOnPerDay = 3600;
const int secondsInDay = 3600 * 24;
int secondsElapsed = 0;
int secondsOn = 0;

double vin;

void loop() {
  delay(interval*1000);
  secondsElapsed += interval;
  if (secondsElapsed > secondsInDay) {
    secondsElapsed = 0;
  }
  if (relayIsOn) {
    secondsOn += interval;
    Serial.println("relay is on");
  } else  Serial.println("relay is off");


  vin = A0toV(readA0());
  printf("seconds elapsed = %d, secondsOn=%d, maxSecondsOnPerDay =%d,  vin = %f\n", secondsElapsed, secondsOn, maxSecondsOnPerDay, vin);

  if ((secondsElapsed < maxSecondsOnPerDay) and (relayIsOn==false) and (vin > vOn) ){
      printf("turning relay on");
      relayOn();
    }

  if (relayIsOn and ((vin < vOff) or (secondsOn > maxSecondsOnPerDay))) {
    printf("turning realy off");
    relayOff();
  }
}
