#include <Arduino.h>
#include "Config.h"
#ifndef fileutils_h
#include "./fileutils.h"
#endif
Config::Config() {

	if (!LittleFS.exists(configFilename)) {
		Serial.println("File" + configFilename + " not found - creating it with defaults");
		save();

	} else {
		Serial.println("Found file " + configFilename);
		readFile(LittleFS, configFilename.c_str());
	}
}

void Config::save() {
	writeFile(LittleFS, configFilename.c_str(), toJson().c_str());
}

void Config::update(JsonObject config) {
	// Access JSON elements like:
	interval = config["interval"].as<int>();
	vOn = config["vOn"].as<double>();
	vOff = config["vOff"].as<double>();
	// Serial.printf("interval: %s, vOn: %s\n", interval.c_str(), vOn.c_str());
	maxSecondsOnPerDay = config["maxSecondsOnPerDay"].as<int>();
	vcal = config["vcal"].as<double>();
	numSamples = config["numSamples"].as<int>();
	isManual = config["isManual"].as<int>();
}


void Config::load() {
	File file = LittleFS.open(configFilename.c_str(), FILE_READ);
	StaticJsonDocument<512> doc;
	DeserializationError error = deserializeJson(doc, file);
	if (error)
		Serial.println(F("Failed to read file, using default configuration"));
	else {
		interval = doc["interval"];
		vOn = doc["vOn"];
		vOff = doc["vOff"];
		maxSecondsOnPerDay = doc["maxSecondsOnPerDay"];
		vcal = doc["vcal"];
		numSamples = doc["numSamples"];
		isManual = doc["isManual"];

		Serial.println("updated config to + toJson()");
	}
}

String Config::modeToString() {
	if (isManual)
		return "on";
	else return "off";
}

String Config::toJson() {
	StaticJsonDocument<512> doc;
	doc["interval"] = interval;
	doc["vOn"] = vOn;
	doc["vOff"] = vOff;
	doc["maxSecondsOnPerDay"] = maxSecondsOnPerDay;
	doc["vcal"] = vcal;
	doc["numSamples"] = numSamples;
	doc["isManual"] = isManual;

	String s;
	serializeJson(doc, s);

	return s;
}