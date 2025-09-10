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
//#include ESPAsyncWebSrv.h
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
#include <ElegantOTA.h>
#include <WebSerial.h>
#include "common.h"

#include <NTPClient.h>
#include <WiFiUdp.h>

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;


const int secondsInDay = 3600 * 24;

//config
Config c;

//state variables
double vin = 0;
int secondsElapsed = 0;
int secondsOn = 0;
time_t _now = 0;
boolean relayIsOn;
boolean relay2IsOn;
//29 June 2024
long dateTime = 1719685735;  //29 June 2024

//I think lcd needs d2
const int relay1Pin = D6;  //!! check board wiring
const int relay2Pin = D5;  //d??
const int AC_DETECT = D7;  //need to assign a digital inpu to this
const int AC_LED = D3;
const int relay2_LED = D8;
// Replace with your network credentials

const int analogInPin = A0;  // ESP8266 Analog Pin ADC0 = A0
String dataFile0 = "/voltageHistory.txt";
String dataFile1 = "/stateHistory.txt";
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


void recvMsg(uint8_t *data, size_t len){
  WebSerial.println("Received Data...");
 
}


/*
add to loop:
if (AC is off) and (V_dc >12)
 then reset relay2
*/

double A0toV(double _a0) {
  Serial.printf("a0=%f, v= %f, v_cal=%f", _a0, _a0 / c.vcal, c.vcal);
  //logInfo();
  return _a0 / c.vcal;
}


int readA0() {

  // read the analog in value
  int a = analogRead(analogInPin);
  if (isnan(a)) {
    logInfo("Failed to read from A0 sensor!");
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
 // digitalWrite(relay1_LED, HIGH);
  relayIsOn = true;
  saveRelayState();
}

void relayOff() {
  digitalWrite(relay1Pin, HIGH);
  //digitalWrite(relay1_LED, LOW);
  relayIsOn = false;
  saveRelayState();
}


void relay2On() {
  digitalWrite(relay2Pin, LOW);
  digitalWrite(relay2_LED, HIGH);
  relay2IsOn = true;
  saveRelayState();
}

void relay2Off() {
  digitalWrite(relay2Pin, HIGH);
  digitalWrite(relay2_LED, LOW);
  relay2IsOn = false;
  saveRelayState();
}

void toggleRelay() {
  saveRelayState();
  if (relayIsOn) {
    relayOff();
  } else relayOn();
  displayStatus();
}

void toggleRelay2() {
  saveRelayState();
  if (relay2IsOn) {
    relay2Off();
  } else relay2On();
  displayStatus();
}

void saveRelayState() {
  //save to file
  logInfo("saving relay state");
  String rdata = "[" + epochToStringms(dateTime) + "," + String(relayIsOn) + "],";
  // to do add relay 2
  appendFile(LittleFS, dataFile1.c_str(), rdata.c_str());
}

boolean getACStatus() {
  //  ac coupler returns high when AC detected, lo when not
  boolean status = !digitalRead(AC_DETECT);
  if (status) { 
    digitalWrite(AC_LED,HIGH);
  }else {
    digitalWrite(AC_LED,LOW);
  }
  return status;
}


String getTimeElapsed() {
  String data = "{\"secondsElapsed\":" + String(secondsElapsed) + ",\"secondsOn\":" + String(secondsOn) + "}";
  return data;
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
    logInfo("LittleFS Mount Failed, trying to format..");
    if (!LittleFS.format()) {
      logInfo("LittleFS Mount Failed");
      return;
    }

  } else {
    logInfo("Little FS Mounted Successfully");
  }
  if (!LittleFS.exists(dataFile0)) {
    logInfo("File" + dataFile0 + "not found - creating it");
    writeFile(LittleFS, dataFile0.c_str(), "");
  } else {
    logInfo("Found file" + dataFile0);
  }

  if (!LittleFS.exists(dataFile1)) {
    logInfo("File" + dataFile1 + "not found - creating it");
    writeFile(LittleFS, dataFile1.c_str(), "");
  } else {
    logInfo("Found file" + dataFile1);
  }

  listAllFilesInDir("/");
  listAllFilesInDir("/plugin/");
}

const char* PARAM_MESSAGE = "message";


void setupWebServer() {
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    logInfo("sending html..");
    request->send(LittleFS, "/index.html");
  });

  server.on("/plugin/json-ui.jquery.js", HTTP_GET, [](AsyncWebServerRequest* request) {
    logInfo("sending html..");
    request->send(LittleFS, "/plugin/json-ui.jquery.js");
  });

  server.on("/plugin/json-ui.css", HTTP_GET, [](AsyncWebServerRequest* request) {
    logInfo("sending html..");
    request->send(LittleFS, "/plugin/json-ui.css");
  });


  server.on("/GET_VOLT_HISTORY", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", readVoltageData().c_str());
  });
  server.on("/GET_STATE_HISTORY", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", readStatusData().c_str());
  });
  server.on("/CLEAR_VOLT_HISTORY", HTTP_GET, [](AsyncWebServerRequest* request) {
    logInfo("clearing voltage data...");
    request->send(200, "text/plain", clearVoltageHistory());
  });
  server.on("/CLEAR_RELAY_STATE_HISTORY", HTTP_GET, [](AsyncWebServerRequest* request) {
    logInfo("clearing relay statedata...");
    clearRelayStateHistory();
    request->send(200, "text/plain", clearRelayStateHistory());
  });
  server.on("/TOGGLE_RELAY", HTTP_GET, [](AsyncWebServerRequest* request) {
    toggleRelay();
    request->send(200, "text/plain", booleanToOnOff(relayIsOn).c_str());
  });

  server.on("/TOGGLE_RELAY2", HTTP_GET, [](AsyncWebServerRequest* request) {
    toggleRelay2();
    request->send(200, "text/plain", booleanToOnOff(relay2IsOn).c_str());
  });

  server.on("/GET_RELAY_STATE", HTTP_GET, [](AsyncWebServerRequest* request) {
    String data = "{\"x\":" + epochToStringms(dateTime) + ",\"y\":" + String(relayIsOn) + 
    ",\"state\":\"" + booleanToOnOff(relay2IsOn) + "\"" +
     ",\"AC\":\"" + booleanToOnOff(getACStatus()) + "\"}";
    request->send(200, "text/plain", data.c_str());
  });

  server.on("/GET_VOLTAGE", [](AsyncWebServerRequest* request) {
   
    String data = "{\"x\":" + epochToStringms(dateTime) + ",\"y\":" + String(vin) + "}";
    request->send(200, "text/plain", data.c_str());
    logInfo("get voltage returned " + data);
  });

  server.on("/GET_CONFIG", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", c.toJson());
  });

  server.on("/GET_ELAPSED", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", getTimeElapsed());
  });

  server.addHandler(new AsyncCallbackJsonWebHandler("/SAVE_CONFIG", [](AsyncWebServerRequest* request, JsonVariant& json) {
    logInfo("received save config request");
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
  ElegantOTA.begin(&server);
  WebSerial.begin(&server);
  WebSerial.onMessage(recvMsg);
  server.begin();
}

void displayStatus() {

  String ACStatus;
  if (getACStatus()) {
    ACStatus = "on";
  } else {
    ACStatus = "off";
  }

  String msg1 = String(secondsOn) + "/" + String(c.maxSecondsOnPerDay) + " AC=" + ACStatus;
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
   pinMode(relay2_LED, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(AC_DETECT, INPUT);
  pinMode(AC_LED, OUTPUT);
  //relayOff();
  getACStatus();


  boolean wifiOK = setupWiFi();
  
  setupLCD();
  lcdDisplayStatus("Pump Controller", "Connecting to wifi....");
  if (wifiOK) {
    lcdDisplayStatus("wi fi connected", "ip=" + WiFi.localIP().toString());
  } else {
    lcdDisplayStatus("wifi connection failed", "");
  }

  setupLittleFS();
  //LittleFS.format();
  setupWebServer();

  logInfo("hello from PumpController");

  c = Config();
  logInfo("loading config");
  c.load();
  logInfo(c.toJson().c_str());

  // Initialize a NTPClient to get time
  timeClient.begin();
  timeClient.setTimeOffset(43200);
}

int timeClientRetryCount = 0;
boolean ACisOn;
void loop() {
  ElegantOTA.loop();
  
  logInfo("loop start");

  timeClientRetryCount = 0;
  while(!timeClient.update()  && (timeClientRetryCount<10)) {
    logInfo(".");
    timeClient.forceUpdate();
    timeClientRetryCount++;

  }
  logInfo("time=" +timeClient.getFormattedDate() );


  digitalWrite(LED_BUILTIN, HIGH);

  delay(c.interval * 1000);

  // get the time (convert to ms for chart)
   dateTime = timeClient.getEpochTime();
 
  logInfo("date time (ms since Jan1 1970)= " + epochToStringms(dateTime));
  // read the data
  vin = A0toV(readA0Avg(c.numSamples));

  //save to file
  String data = "[" + epochToStringms(dateTime) + "," + String(vin) + "],";
  appendFile(LittleFS, dataFile0.c_str(), data.c_str());

  Serial.println(WiFi.localIP());

  logInfo("seconds elapsed = " + String(secondsElapsed) + ", secondsOn=" + String(secondsOn) + ", maxSecondsOnPerDay =" + String(c.maxSecondsOnPerDay) + ",  vin = " + String(vin, 2));

  secondsElapsed += c.interval;
  if (secondsElapsed > secondsInDay) {
    secondsElapsed = 0;
  }

  if (relay2IsOn) {
    secondsOn += c.interval;
  }

  ACisOn = getACStatus();

  if (relay2IsOn and (secondsOn > c.maxSecondsOnPerDay)) {
    if logInfo("turning pump off because time is up);

    relayOff();
  }

  if (relay2IsOn and ((vin < c.vOff) ) {
    if logInfo("turning pump off because battery volsyg is too low");

    relayOff();
  }

  if (relay2IsOn  and !ACisOn() {
     logInfo("turning pump relay off becasuse AC is Off");
    relayOff();

  }

  if ((secondsElapsed < c.maxSecondsOnPerDay) and (relay2IsOn == false) and (vin > c.vOn) and ACisOn) {
    logInfo("turning pump on");
    relayOn();
  }

  

  displayStatus();
}
