
#include<Arduino.h>
#include <NMEAGPS.h>
// #include<Adafruit_GPS.h>
#include <GPSport.h>
#include <Streamers.h>
#define GPSSerial Serial1

static NMEAGPS gps;
static gps_fix fix;


static void doSomeWork()
{
  // Print all the things!

  trace_all( DEBUG_PORT, gps, fix );

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

char buffer[64];
char time[8];
char date[8];
char lon[12];
char lat[12];
char spd[8];
char alt[8];
char sat[2];

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

//--------------------------
// }

void loop()
{
  GPSloop();
}

