#include <Arduino.h>
#include <Adafruit_GPS.h>
#include <RH_RF95.h>
#include <Adafruit_SleepyDog.h>
#include <SoftwareSerial.h>

#define GPSSerial Serial1
#define LED 13
#define VBATPIN A9
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define RF95_FREQ 903.0

Adafruit_GPS GPS(&GPSSerial);
RH_RF95 rf95(RFM95_CS, RFM95_INT);

char buffer[128];
float txFreq = 903.0;
unsigned long heartBeatTime = 5000; // Heartbeat interval in milliseconds
unsigned long lastHeartbeat = 0;
bool gpsFix = false;
void gpsInterrupt();
void send_data(const char *message);

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); // Minimum recommended data

  if (!rf95.init()) {
    Serial.println("LoRa init failed");
    while (1);
  }
  
  rf95.setFrequency(txFreq);
  rf95.setTxPower(23, false);
  rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096); // Bandwidth = 125 kHz, CodingRate 4/8, SpreadingFactor 4096

  Serial.println("LORA Module Initiated");

  Watchdog.enable(5000); // Enable watchdog with a 5-second timeout

  attachInterrupt(digitalPinToInterrupt(GPSSerial.available()), gpsInterrupt, RISING);
}

void loop() {
  if (GPS.newNMEAreceived()) {
    if (GPS.parse(GPS.lastNMEA())) {
      gpsFix = GPS.fix;
      if (gpsFix) {
        snprintf(buffer, sizeof(buffer), "Time: %lu, Lat: %f, Lon: %f, Alt: %f, Speed: %f",
                 GPS.seconds, GPS.latitudeDegrees, GPS.longitudeDegrees, GPS.altitude, GPS.speed);
        send_data(buffer);
      }
    }
  }

  unsigned long currentMillis = millis();
  if (!gpsFix && (currentMillis - lastHeartbeat >= heartBeatTime)) {
    lastHeartbeat = currentMillis;
    snprintf(buffer, sizeof(buffer), "Heartbeat: No GPS Fix");
    send_data(buffer);
  }

  Watchdog.reset(); // Reset the watchdog timer
}

void gpsInterrupt() {
  // Indicate that new data is available from GPS
  if (GPSSerial.available()) {
    GPS.read();
  }
}

void send_data(const char *message) {
  digitalWrite(LED, HIGH);
  rf95.send((uint8_t *)message, strlen(message));
  Serial.print("Sending: ");
  Serial.println(message);
  digitalWrite(LED, LOW);
  delay(50); // Small delay to ensure message is sent
}

float check_bat() {
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;
  measuredvbat *= 3.3;
  measuredvbat /= 1024;
  return measuredvbat;
}
