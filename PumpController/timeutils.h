#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <iostream>
#include <string>
#include <iomanip>  // For std::get_time
#include <ctime>    // For std::tm and mktime
#include <sstream>



std::string time_tToString(time_t t);
time_t timeStrToEpoch(std::string timeString);
time_t getTime();