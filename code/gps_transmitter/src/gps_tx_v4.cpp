//Define serial port
#include<Arduino.h>
// #include<Adafruit_GPS.h>
// #include<TinyGPS++.h>
#define GPSSerial Serial1

const byte numChars = 100;
char receivedChars[numChars];   // an array to store the received data
char gps_string[100];
boolean newData = false;

void setup(){
    while(!Serial);
    Serial.begin(115200);
    GPSSerial.begin(9600);
    // GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_GGAONLY);
    
}

void loop(){
    // Serial.println("Point 1");
    void recvWithEndMarker();
    void showNewData();
    // Serial.println("Point 2");
    if (Serial.available()) {
        char c = Serial.read();
        GPSSerial.write(c);
    }
    // Serial.println("Point 3");
    if (GPSSerial.available()){
        char c =GPSSerial.read();
        Serial.write(c);
    }
}

void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
    Serial.println("recvWithEndMarker Has Run");
   
    while (GPSSerial.available() > 0 && newData == false) {
        rc = GPSSerial.read();

        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}

void showNewData() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        newData = false;
    }
}