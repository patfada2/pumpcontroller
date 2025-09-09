#ifndef timeutils_h
#define timeutils_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <iostream>
#include <string>
#include <iomanip>  // For std::get_time
#include <ctime>    // For std::tm and mktime
#include <sstream>



std::string time_tToString(time_t t);
String  epochToStringms(long epoch);
time_t timeStrToEpoch(std::string timeString);
time_t getTime();

#endif