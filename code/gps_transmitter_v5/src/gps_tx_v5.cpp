//This was an attempt to use an interrupt based approach with NEOGPS and NEOSerial. It never worked. V4 was the best I could do.

#include <Arduino.h>
#include <NMEAGPS.h>
#include <GPSport.h>
#include <Streamers.h>
#include <SPI.h>
#include <RH_RF95.h>
// #include <NeoHWSerial.h> //Couldn't get this library to work the system could never solve the debugport, 
#define GPSSerial Serial1 //if using NeoHWSerial this should be NEO serial
#define LED 13
#define VBATPIN A9
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define RF95_FREQ 903.0
// From NMEA_order:
// Sentence order in each 1-second interval:
//   GGA
//   GSA
//   RMC
//   VTG

static NMEAGPS gps;
static gps_fix fix;
RH_RF95 rf95(RFM95_CS, RFM95_INT);
char buffer[128];
float txFreq = 903.0;
void send_data(char message [128]);
float check_bat ();

//--------------------------

static void GPSisr( uint8_t c )
{
  gps.handle( c );

} // GPSisr

//--------------------------


void setup() {
  DEBUG_PORT.begin(9600);
  DEBUG_PORT.println("GPS_TX_V5");

  // manual reset
  digitalWrite(RFM95_RST, HIGH);
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
 if(!rf95.init())
    Serial.println("LoRa init failed");
  
  rf95.setFrequency(txFreq);
  rf95.setTxPower(23, false);
  Serial.println("LORA Module Initiated");
  // gpsPort.attachInterrupt( GPSisr );
  gpsPort.begin( 9600 );

}

void loop() 
{
   if (gps.available()) {
    // Print all the things!
    trace_all( DEBUG_PORT, gps, gps.read() );
  }

  if (gps.overrun()) {
    gps.overrun( false );
    DEBUG_PORT.println( F("DATA OVERRUN: took too long to print GPS data!") );
  }
}

/*
Sample of how to enable different sentences in the script
 gps.send_P( &gpsPort, F("PUBX,40,GLL,0,0,0,0,0,0") );
  gps.send_P( &gpsPort, F("PUBX,40,GSV,0,0,0,0,0,0") );
  gps.send_P( &gpsPort, F("PUBX,40,GSA,0,0,0,0,0,0") );
  gps.send_P( &gpsPort, F("PUBX,40,VTG,0,1,0,0,0,0") );  // <-- only enabled sentence
  gps.send_P( &gpsPort, F("PUBX,40,ZDA,0,0,0,0,0,0") );
  gps.send_P( &gpsPort, F("PUBX,40,RMC,0,0,0,0,0,0") );
  gps.send_P( &gpsPort, F("PUBX,40,GGA,0,0,0,0,0,0") );
*/
 