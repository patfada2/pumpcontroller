#include <Arduino.h>
#include "Config.h"
Config::Config() {
	Serial.println("Config constructor");
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