//Define serial port
#include<Arduino.h>
#include<Adafruit_GPS.h>
#define GPSSerial Serial1

void setup(){
    while(!Serial);

    Serial.begin(115200);
    
    GPSSerial.begin(9600);
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_GGAONLY);
}

void loop(){
    if (Serial.available()) {
        char c = Serial.read();
        GPSSerial.write(c);
    }

    if (GPSSerial.available()){
        char c =GPSSerial.read();
        Serial.write(c);
    }
}