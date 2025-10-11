#include "common.h"
boolean LOG_TRACE=false;
void logInfo(String msg) {
  Serial.println(msg);
  WebSerial.println(msg);
}

void logTrace(String msg) {
  if (LOG_TRACE) {
    Serial.println(msg);
    WebSerial.println(msg);
  }
}

String formatNumberWithCommas(long number) {
  String result = "";
  String numStr = String(abs(number));  // Convert to string and handle absolute value
  int len = numStr.length();
  int count = 0;

  for (int i = len - 1; i >= 0; i--) {
    result = numStr.charAt(i) + result;
    count++;
    if (count % 3 == 0 && i != 0) {  // Add comma every 3 digits, not at the beginning
      result = "," + result;
    }
  }

  if (number < 0) {
    result = "-" + result;  // Add back the negative sign if original number was negative
  }

  return result;
}
