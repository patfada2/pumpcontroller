#include "common.h"
#include "fileutils.h"
#include "logutils.h"

boolean LOG_TRACE = false;

void setupLogFile() {

  if (!LittleFS.exists(LOG_FILE)) {
    logInfo("File" + LOG_FILE + "not found - creating it");
    writeFile(LittleFS, LOG_FILE.c_str(), "");
  } else {
    logInfo("Found file" + LOG_FILE);
  }
}

void writeLog(String msg) {
  appendFile(LittleFS, LOG_FILE.c_str(), msg.c_str());
}

void logInfo(String msg) {
  Serial.println(msg);
  writeLog(msg);
}

void logTrace(String msg) {
  if (LOG_TRACE) {
    Serial.println(msg);
    writeLog(msg);
  }
}
