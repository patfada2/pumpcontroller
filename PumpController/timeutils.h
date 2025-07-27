#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define TEST_HOST "worldclockapi.com"

HTTPClient http;  // Declare an object of class HTTPClient


std::string time_tToString(time_t t) {
  std::stringstream ss;
  ss << t;
  std::string ts = ss.str();
  return ss.str();
}


