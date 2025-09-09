# Pump Controller
To load index.html to esp, from Arduino Ide
1. disconneserct ial monitor
2. shift ctrl p, select "upload little fs to pico/ESP8286/ESP32

To do

## logs
http://192.168.1.99/webserial

## OTA update
http://192.168.1.99/updates

compile bin file from arduino from sketch>export menu

see https://randomnerdtutorials.com/esp32-ota-elegantota-arduino/

##NTP
time comes from an NTP server- requires connection to internet
 see https://randomnerdtutorials.com/esp32-ntp-client-date-time-arduino-ide/
 note it uses  fork of NTPClient installed from zip


## to do
enable lcd from config

switch to main NTPClient lib

graceful degredation if internet not available (NTP fails)

show ac status on UI

show relay2 status on UI

add menu item to go to /webserial

add menu item to go to /webupdates





