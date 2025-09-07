#include "common.h"

void logInfo(String msg){
	Serial.println(msg);
  	WebSerial.println(msg);	
}