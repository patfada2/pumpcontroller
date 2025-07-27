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

#include <LiquidCrystal_I2C.h>
#include <ESP8266HTTPClient.h>
#include "./timeutils.h"
#include "./fileutils.h"
#include <TimeLib.h>

time_t getTime() {


  WiFiClient client;
  HTTPClient http;

  http.begin(client, "http://worldclockapi.com/api/json/utc/now");

  Serial.print("[HTTP] GETting data...");
  int httpCode = http.GET();  // Send the HTTP GET request

  if (httpCode > 0) {  // Check for a successful HTTP response code
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      String payload = http.getString();  // Get the response payload
      Serial.println(payload);            // Print the payload to Serial Monitor
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

DynamicJsonDocument doc(1024);

  DeserializationError error = deserializeJson(doc, http.getString());
  
  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return 0;
  }
  JsonObject obj = doc.as<JsonObject>();
 String currentDateTime = obj["currentDateTime"];
  Serial.println("currentDateTime="+currentDateTime);
  http.end();  // Close the connection

  int year, month, day, hour, minute, second;

  sscanf(currentDateTime.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
  
  tmElements_t tm;
  int Year, Month, Day, Hour, Minute, Second;
  tm.Year = CalendarYrToTm(Year);
  tm.Month = Month;
  tm.Day = Day;
  tm.Hour = Hour;
  tm.Minute = Minute;
  tm.Second = Second;
  //time_t date = mktime(tm)*1000;
  //time_t makeTime(const tmElements_t &tm);  // convert time elements into time_t
  time_t date = makeTime(tm)*1000;

  return date;
}

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
String dataFile0 = "/a0_7.txt";
String dataFile1 = "/stateHistory.txt";
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

double vin = 0;

//I thinl lcd needs d2
const int relay1Pin = 4;  //d2
const double vcal = 44.0;
boolean relayIsOn;

const int relay2Pin = 0;  //d??
const int AC_DETECT = 0;  //need to assign a digital inpu to this

/*
add to loop:
if (AC is off) and (V_dc >12)
 then reset relay2
*/

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
  digitalWrite(relay1Pin, LOW);
  digitalWrite(LED_BUILTIN, HIGH);
  relayIsOn = true;
  saveRelayState();
}

void relayOff() {
  digitalWrite(relay1Pin, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
  relayIsOn = false; 
  saveRelayState();
}

void saveRelayState() {
   //save to file
  std::string rdata = "[" + time_tToString(dateTime) + "," + std::to_string(relayIsOn) + "],";
  appendFile(LittleFS, dataFile1.c_str(), rdata.c_str());
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

String readData() {
  return readDataFile(dataFile0);
}

String readStatusData() {
  return readDataFile(dataFile1);
}

//return data file as json array
String readDataFile(String path) {
  String s = "";
  int numLines = 0;
  String line = "";
  s = "[";
  Serial.printf("Reading file: %s\r\n", path);

  File file = LittleFS.open(path, FILE_READ);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return "";
  }

  Serial.println("- read from file:");
  while (file.available()) {
    line = (file.readString());
    Serial.println(line);
    s += line;
    numLines += 1;
  }
  file.close();
  //replace traileng comma with ']'
  s[s.length() - 3] = ' ';
  s[s.length() - 2] = ' ';
  s[s.length() - 1] = ']';

  Serial.println("read " + String(numLines) + " lines");
  Serial.println(s);
  return s;
}

String clearData() {
  //delete and recreate the file
  LittleFS.remove(dataFile0.c_str());
  writeFile(LittleFS, dataFile0.c_str(), "");
  return "data cleared";
}
//a0: 0V = 0
//VCC =1024 =3.2

void setup() {

   // Serial port for debugging purposes
  Serial.begin(115200);
  delay(1000);
  
  pinMode(LED_BUILTIN, OUTPUT);
  //d1 = gpo5
  pinMode(relay1Pin, OUTPUT);
  //relayOff();

  lcd.init();

  // turn on LCD backlight
  lcd.backlight();
  lcdIsLineZero = true;
  lcdWrite("PumpController");
 
  // Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println("LittleFS Mount Failed, trying to format..");
    if (!LittleFS.format()) {
      Serial.println("LittleFS Mount Failed");
      return;
    }

  } else {
    Serial.println("Little FS Mounted Successfully");
  }

  // Connect to Wi-Fi
  // Set your Static IP address
  IPAddress local_IP(192, 168, 1, 99);
  // Set your Gateway IP address
  IPAddress gateway(192, 168, 1, 254);

  IPAddress subnet(255, 255, 255, 0);
  IPAddress primaryDNS(8, 8, 8, 8);    //optional
  IPAddress secondaryDNS(8, 8, 4, 4);  //optional

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println( WiFi.localIP());


   if (!LittleFS.exists(dataFile0)) {
    Serial.println("File" + dataFile0 + "not found - creating it");
    writeFile(LittleFS, dataFile0.c_str(), "");
  } else {
    Serial.println("Found file" + dataFile0);
    //readFile(LittleFS, dataFile0.c_str());
  }
  
  if (!LittleFS.exists(dataFile1)) {
    Serial.println("File" + dataFile1 + "not found - creating it");
    writeFile(LittleFS, dataFile1.c_str(), "");
  } else {
    Serial.println("Found file" + dataFile1);
    //readFile(LittleFS, dataFile0.c_str());
  }

  listAllFilesInDir("/");

  Serial.println("hello from PumpController");

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("sending html..");
    request->send(LittleFS, "/index.html");
  });
  server.on("/GET_DATA", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", readData().c_str());
  });
  server.on("/GET_STATUS", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", readStatusData().c_str());
  });
  server.on("/CLEAR_DATA", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("clearing data...");
    clearData();
    request->send(200, "text/plain", "cleared");
  });

  server.on("/GET_VOLTAGE", [](AsyncWebServerRequest* request) {
    //std::string data = "{\"x\":" + time_tToString(dateTime) + ",\"y\":" + std::to_string(vin) + "}";
    //request->send_P(200, "text/plain", data.c_str());
    String data = "{\"x\":" + String(dateTime) + ",\"y\":" + String(vin) + "}";
    //+ time_tToString(dateTime) + ",\"y\":"  + "vv" + "}";
    //request->send_P(200, "text/plain", data.c_str());
    request->send(200, "text/plain", data.c_str());

    Serial.println("get voltage returned " + data);
  });

  // Start server
  server.begin();
}

const int interval = 2;  //sampling interval in seconds
const double vOn = 13.3;
const double vOff = 11.0;
const int maxSecondsOnPerDay = 4800;
const int secondsInDay = 3600 * 24;
int secondsElapsed = 0;
int secondsOn = 0;

time_t _now;
std::string data;

void loop() {
  Serial.println("loop");

  digitalWrite(LED_BUILTIN, HIGH);

  delay(interval * 1000);

  // get the time
  getTime();
  _now = 0;
  if (_now > 0) {
    dateTime = _now;
  } else {
    Serial.println("estimating date time");
    dateTime += interval;
  }
  Serial.println("date time = " + String(dateTime));
  // read the data
  vin = A0toV(readA0());

  //save to file
  data = "[" + time_tToString(dateTime) + "," + std::to_string(vin) + "],";
  appendFile(LittleFS, dataFile0.c_str(), data.c_str());

  Serial.println(WiFi.localIP());
  lcdWrite("checking..");

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
   Serial.println("housekeeping");
  
}
