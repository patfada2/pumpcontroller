#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <iostream>
#include <string>
#include <iomanip>  // For std::get_time
#include <ctime>    // For std::tm and mktime
#include <sstream>

#define TEST_HOST "worldclockapi.com"

HTTPClient http;  // Declare an object of class HTTPClient


std::string time_tToString(time_t t) {
  std::stringstream ss;
  ss << t;
  std::string ts = ss.str();
  return ss.str();
}


time_t timeStrToEpoch(std::string timeString) {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;

  std::tm t{};
  std::istringstream ss(timeString);


  // Parse the string into the tm structure using a specified format
  // The format string must match the input string's format
  ss >> std::get_time(&t, "%Y-%m-%dT%H:%MZ");

  if (ss.fail()) {
    std::cerr << "Failed to parse time string!" << std::endl;
    return 1;
  }

  // Convert the tm structure to a time_t timestamp
  // mktime accounts for local time zone and DST
  std::time_t timestamp = mktime(&t);

  if (timestamp == -1) {
    std::cerr << "Failed to convert tm to time_t!" << std::endl;
    return 1;
  } else return timestamp*1000;
}



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
  Serial.println("currentDateTime=" + currentDateTime);
  http.end();  // Close the connection

  return timeStrToEpoch(currentDateTime.c_str());
}