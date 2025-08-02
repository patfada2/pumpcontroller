#include <Arduino.h>
#include "Config.h"
#ifndef fileutils_h
#include "./fileutils.h"
#endif 
Config::Config() {
	
	Serial.println("Config constructor");
	if (!LittleFS.exists(configFilename)) {
    Serial.println("File" + configFilename + "not found - creating it");
    writeFile(LittleFS, configFilename.c_str(), "");
  } else {
    Serial.println("Found file" + configFilename);
  }

}

void Config::save() {

}

String Config::toJson() {
	DynamicJsonDocument doc(200);  // For a larger, dynamic JSON document
	doc["interval"] = interval;

	String s;
	serializeJson(doc, s);

	return s;
}