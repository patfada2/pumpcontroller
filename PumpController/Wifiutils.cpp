#include <Arduino.h>
#include <ArduinoJson.h>
#include "wifiutils.h"
#include "common.h"




//const char* ssid = "COMFAST";
const char* ssid = "RosieWiFi";
const char* password = "Thr33.0n3";


boolean setupWiFi() {

  boolean result = false;

  // Connect to Wi-Fi
  // Set your Static IP address
  //IPAddress local_IP(192, 168, 10, 99);
  IPAddress local_IP(192, 168, 1, 99);
  // Set your Gateway IP address
 // IPAddress gateway(192, 168, 10, 254);
  IPAddress gateway(192, 168, 1, 254);

  IPAddress subnet(255, 255, 255, 0);
  IPAddress primaryDNS(8, 8, 8, 8);    //optional
  IPAddress secondaryDNS(8, 8, 4, 4);  //optional

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
    result = false;
  }

  WiFi.begin(ssid, password);
  int retry = 0;
  while ((WiFi.status() != WL_CONNECTED) && retry <10 ){
    delay(1000);
    retry++;
    logInfo("Connecting to WiFi..");
  }
  if (WiFi.status() == WL_CONNECTED) {
    result = true;
    Serial.println(WiFi.localIP());
  } else {
    result = false;
    logInfo("failed to connect to wifi ") ;
  }
  // Print ESP32 Local IP Address
  
  return result;
}

