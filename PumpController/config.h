#ifndef Config_h
#define Config_h
#include "Arduino.h" 
//#include "./fileutils.h"
 #include <ArduinoJson.h>

const String configFilename = "/config.json";

class Config {
// class code goes here
public:
	int interval = 5;  //sampling interval in seconds
	double vOn = 13.3;
  double vOff = 11.0;
  int maxSecondsOnPerDay = 4800;
	double vcal = 44.0;  //volatage calibration
	int numSamples=5; //number of time A0 is sampled for average
	
	Config();
	void save();
	void load();
	String toJson();
	
private:
	
};

#endif