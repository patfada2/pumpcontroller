#include <WiFiClientSecure.h>
// For Non-HTTPS requests
 WiFiClient client;

// For HTTPS requests
//WiFiClientSecure client;

#define TEST_HOST "worldtimeapi.org"


time_t getTime()
{

  // Opening connection to server (Use 80 as port if HTTP)
  //http://worldtimeapi.org/api/timezone/Pacific/Auckland", 
  if (!client.connect(TEST_HOST, 80))
  {
    Serial.println(F("Connection failed"));
    return 0;
  }

  // give the esp a breather
  yield();

  // Send HTTP request
  client.print(F("GET "));
  // This is the second half of a request (everything that comes after the base URL)
  client.print("/api/timezone/Pacific/Auckland"); // %2C == ,

  // HTTP 1.0 is ideal as the response wont be chunked
  // But some API will return 1.1 regardless, so we need
  // to handle both.
  client.println(F(" HTTP/1.0"));

  //Headers
  client.print(F("Host: "));
  client.println(TEST_HOST);

  client.println(F("Cache-Control: no-cache"));

  if (client.println() == 0)
  {
    Serial.println(F("Failed to send request"));
    return 0;
  }
  //delay(100);
  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));

  // Check if it responded "OK" with either HTTP 1.0 or 1.1
  if (strcmp(status, "HTTP/1.0 200 OK") != 0 )
  {
    {
      Serial.print(F("Unexpected response: "));
      Serial.println(status);
      return 0;
    }
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(F("Invalid response"));
    return 0;
  }

  // For APIs that respond with HTTP/1.1 we need to remove
  // the chunked data length values at the start of the body
  //
  // peek() will look at the character, but not take it off the queue
  while (client.available() && client.peek() != '{' && client.peek() != '[')
  {
    char c = 0;
    client.readBytes(&c, 1);
    Serial.print(c);
    Serial.println("BAD");
  }

  // While the client is still availble read each
  // byte and print to the serial monitor
  String json="";
  StaticJsonDocument<2000> doc;
  while (client.available())
  {
    char c = 0;
    client.readBytes(&c, 1);  
    json += c;
  }
  Serial.println(json);
   DeserializationError error = deserializeJson(doc, json);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return 0;
  }
  const char* datetime = doc["datetime"];
 

  // Print values.
  Serial.println(datetime);

//convert to timestamp

// Initialize a tm structure to hold the parsed date
tm tm = {};

// Create a string stream to parse the date string
std::istringstream ss(datetime);

// Parse the date string using std::get_time
ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");

// Check if parsing was successful
if (ss.fail()) {
    Serial.println("Date parsing failed!");
    //return 1;
}

// Convert the parsed date to a time_t value
time_t date = mktime(&tm)*1000;
Serial.println(date);
return date;

}
