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
  } else return timestamp;
}