#include <Arduino.h>    
#include <Adafruit_GPS.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <string.h>
using namespace std;
/* for feather32u4 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define LED 13
#define VBATPIN A9
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 868.0

// what's the name of the hardware serial port?
#define GPSSerial Serial1

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);
     
// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

RH_RF95 rf95(RFM95_CS, RFM95_INT);

uint32_t timer = millis();


void send_data(char message [50]);
float check_bat ();


void setup()
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600);
  delay(100);

  Serial.println("Feather LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
    char status_rf95[50] = "0, RF_95 TX INITIATED";
  send_data(status_rf95);
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
  rf95.setModemConfig('Bw125Cr48Sf4096');// Bandwidth = 125 kHz BW, CodingRate 4/8, SpreadingFactor 4096  Slow+long range
  
  //while (!Serial);  // uncomment to have the sketch wait until Serial is ready
  
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  Serial.println("GPS TEST PARSED BEGIN");
     
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz
     
  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA); // explained here: https://learn.adafruit.com/adafruit-ultimate-gps/external-antenna
  char status_gps[50] = "0, GPS INITIATING";
  send_data(status_gps);
  delay(1000);
  
  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);
}
char buffer[55];
char latstr[12];
char lonstr[12];
char altstr[8];
char spdstr[8];
float bat_volt = 0;
char bat_volt_str[5];


void loop() // run over and over again
{
     memset(buffer, '\0', sizeof(buffer));
     memset(latstr, '\0', sizeof(latstr));
     memset(lonstr, '\0', sizeof(lonstr));
     memset(altstr, '\0', sizeof(altstr));
     memset(spdstr, '\0', sizeof(spdstr));

  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }
  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis()) timer = millis();

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 1000) {
    timer = millis(); // reset the timer

    if (GPS.fix) {

//      Serial.print("Location: ");
//      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
//      Serial.print(", ");
//      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
//      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
//      Serial.print("Angle: "); Serial.println(GPS.angle);
      int mess_type = 1; // indicates valid GPS message to RX unit
      int num_sats = GPS.satellites;
      int fix_type = GPS.fixquality_3d;
      bat_volt = check_bat();
      // Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites); 
      Serial.print("Fix Quality: "); Serial.println((int)GPS.fixquality);
      Serial.print("3D Fix: "); Serial.println((int)GPS.fixquality_3d);
      dtostrf(GPS.latitude, 9, 4, latstr); //dtostrf is necessary b/c Arduino doesn't do floats or double in sprintf
      dtostrf(GPS.longitude, 9, 4, lonstr);
      dtostrf(GPS.speed, 5, 2, spdstr);
      dtostrf(GPS.altitude, 5, 2, altstr);
      dtostrf(bat_volt, 3, 2, bat_volt_str);
      // dtostrf(check_bat, 4, 3, bat_volt);
      // Serial.println(lonstr);
     for (int transtrans = 1; transtrans < 5; transtrans = transtrans + 1)
     {
    
      // byte sendLen;
//      sprintf(buffer, "Lat: %s  Lon: %s W", latstr, lonstr);
      sprintf(buffer, "%i, %s, %c, %s, %c, %s, %s, %s, %i, %i ", mess_type, latstr, GPS.lat, lonstr, GPS.lon, altstr, spdstr, bat_volt_str, num_sats, fix_type);
      send_data(buffer);
//      Serial.print("Lat: "); Serial.println(latstr);
      // sendLen= strlen(buffer);
      // Serial.print("Sending: "); Serial.println(buffer);
      // rf95.send((uint8_t*)buffer, sendLen);
      // delay(50);
     }

  
    }
    // else {
    //   int mess_type = 2;
    //   int num_sats = GPS.satellites;
    //   sprintf(buffer, "%i, %i", mess_type, num_sats);
    //   send_data(buffer);
    //  }
  }
}

void send_data(char message [55])
{
  digitalWrite(LED, HIGH);
  byte sendLen ;
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
  Serial.print("VBat: " ); Serial.println(measuredvbat);
  return measuredvbat;
}