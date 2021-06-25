// Simple GPS read and transmit, based on NEOGPS library.
//This is presently the best working version.

#include<Arduino.h>
#include <NMEAGPS.h>
// #include<Adafruit_GPS.h>
#include <GPSport.h>
#include <Streamers.h>
#include <SPI.h>
#include <RH_RF95.h>
// #include <NeoHWSerial.h>
#define GPSSerial Serial1
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
static int source_address = 0x0a;

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
  // int32_t pdop = fix.pdop;
  // int32_t laterr = fix.lat_err_cm;
  // int32_t lonerr = fix.lon_err_cm;
  int16_t battvolt = check_bat()*1000;
  // Serial.println(laterr);
  // int32_t sat = gps;
  // Serial.println(time);
  // Serial.println(lat);
  // Serial.println(lon);
  // Serial.println(alt);
  // Serial.println(spd);
  // Serial.println(pdop);
  // Serial.println(battvolt);
  // sprintf(buffer, "%li, %li, %li, %li, %li, %li, %li, %li, %i", time, lat, lon, alt, spd, pdop, laterr, lonerr, battvolt);
  int mess_type = 0x01;
  sprintf(buffer, "%i, %i, %li, %li, %li, %li, %li, %i", source_address, mess_type, time, lat, lon, alt, spd, battvolt); //pdop and lat/lon were giving errors.
  // Serial.println(buffer);
  send_data(buffer);
  // digitalWrite(LED, LOW);

  
} // doSomeWork

//------------------------------------
//  This is the main GPS parsing loop.

static void GPSloop()
{
  while (gps.available( gpsPort )) {
    fix = gps.read();
    doSomeWork();
  // fix = gps.read();
  // doSomeWork();
  }

  // else {
    
  //   send_data
  // }

} // GPSloop



// const byte numChars = 100;
// char receivedChars[numChars];   // an array to store the received data
// char gps_string[100];
// boolean newData = false;


void setup(){
   DEBUG_PORT.begin(9600);
  // while (!DEBUG_PORT)
  //   ;

  DEBUG_PORT.print( F("GPS_TX_Version 4\n") );

  digitalWrite(RFM95_RST, HIGH);
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);


  if(!rf95.init())
    Serial.println("LoRa init failed");
  
  rf95.setFrequency(txFreq);
  rf95.setTxPower(23, false);
  rf95.setModemConfig('Bw125Cr48Sf4096');// Bandwidth = 125 kHz BW, CodingRate 4/8, SpreadingFactor 4096  Slow+long range
  Serial.println("LORA Module Initiated");
  gpsPort.begin( 9600 );
}


void loop()
{
  // if (gps.available()){
  GPSloop();  
  // }
//   while (gps.available( gpsPort )) {
//     fix = gps.read();
//     doSomeWork();
//     GPSloop();
//   }
// //   else {
// //   int mess_type = 0x10;
// //   // char message[128] = "NO GPS"
// //   sprintf(buffer, "%i, %i, NO GPS", source_address, mess_type);
// //   send_data(buffer);
// //   delay(100);
// }
}

void send_data(char message [128])
{
  digitalWrite(LED, HIGH);
  byte sendLen;
  sendLen = strlen(message);
  rf95.send((uint8_t*)message, sendLen);
  Serial.print("Sending: "); Serial.println(message);
  digitalWrite(LED, LOW);
  delay(50);
}

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