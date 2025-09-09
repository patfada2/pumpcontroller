#include <Arduino.h>
#include <ArduinoJson.h>
#include "timeutils.h"


std::string time_tToString(time_t t) {
  std::stringstream ss;
  ss << t;
  std::string ts = ss.str();
  return ss.str();
}


//takes an epoch in seconds and outputs  string representation of epoch to ms
// ms epoch is too long for the long tyep to handle (32 bits) so we just append "000"

String epochToStringms(long epoch) {
  return String(epoch)+"000";
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

