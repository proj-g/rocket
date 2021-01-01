#include<Arduino.h>
#include <NMEAGPS.h>
// #include<Adafruit_GPS.h>
#include <GPSport.h>
#include <Streamers.h>
#include <RH_RF95.h>
// #include <NeoHWSerial.h>
#define GPSSerial Serial1
#define LED 13
#define VBATPIN A9
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
// From NMEA_order:
// Sentence order in each 1-second interval:
//   GGA
//   GSA
//   RMC
//   VTG

static NMEAGPS gps;
static gps_fix fix;
// int buffer_len = 128;
char buffer[128];
// char time[8];
// char date[8];
// char lon[12];
// char lat[12];
// char spd[8];
// char alt[8];
// char sat[2];

void send_data(char message [128]);
float check_bat ();

static void doSomeWork()
{
  // Print all the things!
  // digitalWrite(LED, HIGH);
  // trace_all( DEBUG_PORT, gps, fix );
  // int32_t time= fix.dateTime; 
  int32_t time = fix.dateTime; 
  int32_t lat = fix.latitudeL();
  int32_t lon = fix.longitudeL();
  int32_t alt = fix.alt.whole;
  int32_t spd = fix.spd.whole;
  int32_t pdop = fix.pdop;
  int32_t laterr = fix.lat_err_cm;
  int32_t lonerr = fix.lon_err_cm;
  // Serial.println(laterr);
  // int32_t sat = gps;
  // Serial.println(time);
  // Serial.println(lat);
  // Serial.println(lon);
  // Serial.println(alt);
  // Serial.println(spd);
  // Serial.println(pdop);
  sprintf(buffer, "%li, %li, %li, %li, %li, %li, %li, %li", time, lat, lon, alt, spd, pdop, laterr, lonerr);
  Serial.println(buffer);
  // send_data(buffer);
  // digitalWrite(LED, LOW);

  
} // doSomeWork

//------------------------------------
//  This is the main GPS parsing loop.

static void GPSloop()
{
  while (gps.available( gpsPort )) {
    fix = gps.read();
    doSomeWork();
  }

} // GPSloop



// const byte numChars = 100;
// char receivedChars[numChars];   // an array to store the received data
// char gps_string[100];
// boolean newData = false;


void setup(){
    // while(!Serial);
    // Serial.begin(115200);
    // GPSSerial.begin(9600);
    // GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_GGAONLY);
   DEBUG_PORT.begin(9600);
  while (!DEBUG_PORT)
    ;

  DEBUG_PORT.print( F("NMEAsimple started\n") );
 

  gpsPort.begin( 9600 );
}


void loop()
{
  GPSloop();
}

// void send_data(char message [128])
// {
//   digitalWrite(LED, HIGH);
//   byte sendLen;
//   sendLen = strlen(message);
//   rf95.send((uint8_t*)message, sendLen);
//   Serial.print("Sending: "); Serial.println(message);
//   digitalWrite(LED, LOW);
//   delay(50);
// }

float check_bat()
{
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
//   Serial.print("VBat: " ); Serial.println(measuredvbat);
  return measuredvbat;
}


//************************************************************************//