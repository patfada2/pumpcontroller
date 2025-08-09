#include <LittleFS.h>

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
#include <TimeLib.h>
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include "./timeutils.h"
#include "./fileutils.h"
#include "./lcdutils.h"
#include "./config.h"
#include "./wifiutils.h"
#include "Config.h"
#include <AsyncJson.h>


const int secondsInDay = 3600 * 24;

//config
Config c;

//state variables
double vin = 0;
int secondsElapsed = 0;
int secondsOn = 0;
time_t _now = 0;
boolean relayIsOn;
//29 June 2024
time_t dateTime = 1719685735000;  //29 June 2024

std::string data;

//I think lcd needs d2
const int relay1Pin = D4;  //!! check board wiring
const int relay2Pin = 0;   //d??
const int AC_DETECT = 0;   //need to assign a digital inpu to this
// Replace with your network credentials

const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0
String dataFile0 = "/voltageHistory.txt";
String dataFile1 = "/stateHistory.txt";
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


/*
add to loop:
if (AC is off) and (V_dc >12)
 then reset relay2
*/

double A0toV(double _a0) {
  Serial.printf("a0=%f, v= %f", _a0, _a0 / c.vcal);
  Serial.println();
  return _a0 / c.vcal;
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

double readA0Avg(int count) {
  double sample = 0;
  for (int i = 0; i < count; i++) {
    sample = sample + readA0();
    delay(10);
  }
  double avg = sample / count;
  Serial.printf("avg=%f", avg);
  return avg;
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

void toggleRelay() {
  saveRelayState();
  if (relayIsOn) {
    relayOff();
  } else relayOn();
  displayStatus();
}
void saveRelayState() {
  //save to file
  Serial.println("saving relay state");
  std::string rdata = "[" + time_tToString(dateTime) + "," + std::to_string(relayIsOn) + "],";
  appendFile(LittleFS, dataFile1.c_str(), rdata.c_str());
}

String booleanToOnOff(boolean flag) {
  if (flag) {
    return "on";
  } else return "off";
}

String readVoltageData() {
  return readDataFile(dataFile0);
}

String readStatusData() {
  return readDataFile(dataFile1);
}

String clearVoltageHistory() {
  //delete and recreate the file
  LittleFS.remove(dataFile0.c_str());
  writeFile(LittleFS, dataFile0.c_str(), "");
  return "voltage history cleared";
}


String clearRelayStateHistory() {
  //delete and recreate the file
  LittleFS.remove(dataFile0.c_str());
  writeFile(LittleFS, dataFile1.c_str(), "");
  return "state history cleared";
}

void setupLittleFS() {

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
  if (!LittleFS.exists(dataFile0)) {
    Serial.println("File" + dataFile0 + "not found - creating it");
    writeFile(LittleFS, dataFile0.c_str(), "");
  } else {
    Serial.println("Found file" + dataFile0);
  }

  if (!LittleFS.exists(dataFile1)) {
    Serial.println("File" + dataFile1 + "not found - creating it");
    writeFile(LittleFS, dataFile1.c_str(), "");
  } else {
    Serial.println("Found file" + dataFile1);
  }

  listAllFilesInDir("/");
  listAllFilesInDir("/plugin/");
}

const char* PARAM_MESSAGE = "message";


void setupWebServer() {
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("sending html..");
    request->send(LittleFS, "/index.html");
  });

  server.on("/plugin/json-ui.jquery.js", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("sending html..");
    request->send(LittleFS, "/plugin/json-ui.jquery.js");
  });

  server.on("/plugin/json-ui.css", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("sending html..");
    request->send(LittleFS, "/plugin/json-ui.css");
  });


  server.on("/GET_VOLT_HISTORY", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", readVoltageData().c_str());
  });
  server.on("/GET_STATE_HISTORY", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", readStatusData().c_str());
  });
  server.on("/CLEAR_VOLT_HISTORY", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("clearing voltage data...");
    request->send(200, "text/plain", clearVoltageHistory());
  });
  server.on("/CLEAR_RELAY_STATE_HISTORY", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("clearing relay statedata...");
    clearRelayStateHistory();
    request->send(200, "text/plain", clearRelayStateHistory());
  });
  server.on("/TOGGLE_RELAY", HTTP_GET, [](AsyncWebServerRequest* request) {
    toggleRelay();
    request->send(200, "text/plain", booleanToOnOff(relayIsOn).c_str());
  });

  server.on("/GET_RELAY_STATE", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", booleanToOnOff(relayIsOn).c_str());
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

  server.on("/GET_CONFIG", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", c.toJson());
  });


  server.addHandler(new AsyncCallbackJsonWebHandler("/SAVE_CONFIG", [](AsyncWebServerRequest* request, JsonVariant& json) {
    Serial.println("received save config request");
    if (json.is<JsonObject>()) {
      JsonObject obj = json.as<JsonObject>();
      c.update(obj);
      c.save();
     
      request->send(200, "application/json", "{\"status\":\"success\"}");
    } else {
      request->send(400, "text/plain", "Invalid JSON format");
    }
  }));


  // Start server
  server.begin();
}

void displayStatus() {


  String msg1 = String(secondsOn) + "/" + String(c.maxSecondsOnPerDay);
  String msg2;

  if (relayIsOn) {
    msg2 = "ON v=" + String(vin, 2);
  } else msg2 = "OFF v=" + String(vin, 2);

  msg2 = msg2 + " t=" + String(secondsElapsed);

  lcdDisplayStatus(msg1, msg2);
}



void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_BUILTIN, OUTPUT);
  //d1 = gpo5
  pinMode(relay1Pin, OUTPUT);
  //relayOff();

  setupLCD();
  setupWiFi();
  setupLittleFS();
  //LittleFS.format();
  setupWebServer();


  _now = getTime();
  while (_now == 0) {
    delay(1000);
    Serial.println("retrying getTime...");
    _now = getTime();
  }

  Serial.println("hello from PumpController");

  c = Config();
  Serial.println("loading config");
  c.load();
  Serial.println(c.toJson().c_str());
}


void loop() {

  Serial.println("loop");

  digitalWrite(LED_BUILTIN, HIGH);

  delay(c.interval * 1000);

  // get the time

  _now = getTime();

  if (_now > 0) {
    dateTime = _now;
  } else {
    Serial.println("estimating date time");
    dateTime += c.interval;
  }
  Serial.println("date time = " + String(dateTime));
  // read the data
  vin = A0toV(readA0Avg(c.numSamples));

  //save to file
  data = "[" + time_tToString(dateTime) + "," + std::to_string(vin) + "],";
  appendFile(LittleFS, dataFile0.c_str(), data.c_str());

  Serial.println(WiFi.localIP());

  Serial.println("seconds elapsed = " + String(secondsElapsed) + ", secondsOn=" + String(secondsOn) + ", maxSecondsOnPerDay =" + String(c.maxSecondsOnPerDay) + ",  vin = " + String(vin, 2));

  secondsElapsed += c.interval;
  if (secondsElapsed > secondsInDay) {
    secondsElapsed = 0;
  }

  if (relayIsOn) {
    secondsOn += c.interval;
  }

  if ((secondsElapsed < c.maxSecondsOnPerDay) and (relayIsOn == false) and (vin > c.vOn)) {
    Serial.println("turning relay on");
    relayOn();
  }

  if (relayIsOn and ((vin < c.vOff) or (secondsOn > c.maxSecondsOnPerDay))) {
    Serial.println("turning relay off");

    relayOff();
  }

  displayStatus();
}
