#include <Arduino.h>
#include <ArduinoJson.h>
#include "wifiutils.h"



const char* ssid = "RosieWiFi";
const char* password = "Thr33.0n3";


void setupWiFi() {

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
  Serial.println(WiFi.localIP());
}

