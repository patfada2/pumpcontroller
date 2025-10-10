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

I cant get the OTA upload to the webfiles working - cant make a valid littlefs bin. 

Have installed mklittlefs:

  mklittlefs -c ./data -d 5  -s 2072576 -b 8192 data.bin


it creates the bin and it will upload, but it doesnt work - no index.html


##NTP
time comes from an NTP server- requires connection to internet
 see https://randomnerdtutorials.com/esp32-ntp-client-date-time-arduino-ide/
 note it uses  fork of NTPClient installed from zip
 
 trying 
 nz.pool.ntp.org
   w32tm /stripchart /computer:nz.pool.ntp.org /dataonly /samples:5


## to do

get voltage returnign 0 after page refresh, clear hsitory...
can we use a lcoal index.html
change geHistory to just return last n points or paginate


enable lcd from config

json structure for file info response

clear history when disk > 80%



create  a static route from rosiewifi to comcast

trying this (when connectted to redmi)

>route add 192.168.10.0 MASK 255.255.255.0 192.168.10.254 -p 

no  joy


C:\Windows\System32>tracert 192.168.10.99

Tracing route to 192.168.10.99 over a maximum of 30 hops

  1     *        *     PatsXPS [192.168.31.243]  reports: Destination host unreachable.

Trace complete.




----
/voltageHistory.txt size: 17904 - stops  working
/voltageHistory.txt size: 18024


