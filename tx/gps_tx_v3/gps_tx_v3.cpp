#include <Arduino.h>    
#include <Adafruit_GPS.h>
#include <SPI.h>
#include <RH_RF95.h>

using namespace std;

/* for feather32u4 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define LED 13
#define VBATPIN A9
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// what's the name of the hardware serial port?
#define GPSSerial Serial1

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

#define GPSECHO false
#define INPUT_BUFFER_LIMIT (128 + 1)

RH_RF95 rf95(RFM95_CS, RFM95_INT);

uint32_t timer = millis();

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
void send_data(char message [50]);
float check_bat ();

void setup()
{
    Serial.begin(9600);
    // RFM 95 Setup
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, HIGH);
    delay(100);
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
   
    if (!rf95.setFrequency(RF95_FREQ)) {
        Serial.println("setFrequency failed");
        while (1);
    }

    Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
    char status_rf95[50] = "0, RF_95 TX INITIATED";
    send_data(status_rf95);
    rf95.setTxPower(23, false);
    rf95.setModemConfig('Bw125Cr48Sf4096');// Bandwidth = 125 kHz BW, CodingRate 4/8, SpreadingFactor 4096  Slow+long range

    //GPS setup
    GPS.begin(9600); //Set Baud rate
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY); // Select Packet format
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate (1hz is recommended rate)
    GPS.sendCommand(PGCMD_ANTENNA); // explained here: https://learn.adafruit.com/adafruit-ultimate-gps/external-antenna
    char status_gps[50] = "0, GPS INITIATING";
    send_data(status_gps);
    delay(1000);
    GPSSerial.println(PMTK_Q_RELEASE);  // Ask for firmware version
}

// Setup global variables
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

    char c = GPS.read();
    if (GPSECHO)
        if (c) Serial.print(c);
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
                sprintf(buffer, "%i, %s, %c, %s, %c, %s, %s, %s, %i, %i ", mess_type, latstr, GPS.lat, lonstr, GPS.lon, altstr, spdstr, bat_volt_str, num_sats, fix_type);
                send_data(buffer);
            }
        }
    }
}

void send_data(char message [55])
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
  Serial.print("VBat: " ); Serial.println(measuredvbat);
  return measuredvbat;
}
