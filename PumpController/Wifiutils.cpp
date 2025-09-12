#include <Arduino.h>
#include <ArduinoJson.h>
#include "wifiutils.h"
#include "common.h"

//#define ROSIE_WIFI
#define COMFAST
const char* password = "Thr33.0n3";
#if defined(ROSIE_WIFI)
const char* ssid = "RosieWiFi";
IPAddress local_IP(192, 168, 1, 99);
IPAddress gateway(192, 168, 1, 254);
#elif defined(COMFAST)
const char* ssid = "COMFAST";
IPAddress local_IP(192, 168, 10, 99);
IPAddress gateway(192, 168, 10, 254);
#else
Serial.println("Wifi network undefined");

#endif

boolean setupWiFi() {
  boolean result = false;

  String msg = "connecting to SSID=" + String(ssid);
  msg = msg + "local_ip="+local_IP.toString();
  Serial.println(msg);

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
  while ((WiFi.status() != WL_CONNECTED) && retry < 10) {
    delay(1000);
    retry++;
    logInfo("Connecting to WiFi..");
  }
  if (WiFi.status() == WL_CONNECTED) {
    result = true;
    Serial.println(WiFi.localIP());
  } else {
    result = false;
    logInfo("failed to connect to wifi ");
  }
  // Print ESP32 Local IP Address

  return result;
}
