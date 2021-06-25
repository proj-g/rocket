#include <Arduino.h>
#include <RH_RF95.h>
#include <SPI.h>

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define RF95_FREQ 903.0
#define rx_led 13
#define tx_led 12

RH_RF95 rf95(RFM95_CS, RFM95_INT);

char buffer[128];
float txFreq = 903.0;
void send_data(char message [128]);
// void receive_data(char incoming [128]);
float check_bat ();
static int source_address = 0x0f;
uint8_t buf[128];
uint8_t len=sizeof(buf);

void setup() {
  Serial.begin(9600);
  Serial.println("Relay V1");

  // RFM 95 Setup
  Serial.println( F("RELAY Version 1") );

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
}

void loop() {
  // put your main code here, to run repeatedly:
  if(rf95.recv(buf, &len))
  {
    // receive_data();
    digitalWrite(rx_led, HIGH);
    Serial.print("Message recieved: ");
    Serial.println((char*)buf);
    Serial.print("RSSI: ");
    Serial.println(rf95.lastRssi(), DEC);
    digitalWrite(rx_led, LOW);
  }


}

void send_data(char message [128])
{
  digitalWrite(tx_led, HIGH);
  byte sendLen;
  sendLen = strlen(message);
  rf95.send((uint8_t*)message, sendLen);
  Serial.print("Sending: "); Serial.println(message);
  digitalWrite(tx_led, LOW);
  delay(50);
}