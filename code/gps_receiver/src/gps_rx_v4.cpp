//Moved to a new Reciever box, this is an early start, and does not work. 

#include<Arduino.h>
#include<RH_RF95.h>
// #include<Adafruit_LiquidCrystal.h>
#include<SPI.h>

#define LED 13

//RFM95 Setup
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
// #define RF95_FREQ 903.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);
float Freq = 903.0;

//LCD Setup
// Adafruit_LiquidCrystal lcd(6, 5, 9, 10, 11, 12);



void setup()
{
  Serial.begin(9600);
  delay(100);
  digitalWrite(RFM95_RST, HIGH);
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  if(!rf95.init())
    Serial.println("LoRa init failed");
  
  rf95.setFrequency(Freq);
  rf95.setTxPower(23, false);
  Serial.println("LORA Module Initiated");
}

void loop()
{
  if (rf95.available())
  {
    uint8_t message[128];
    uint8_t len = sizeof(message);
    if (rf95.recv(message, &len))
    {
      digitalWrite(LED, HIGH);
      Serial.print("Message Recieved: ");
      Serial.println((char*)message);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
    }
    else 
    {
      Serial.println("receive failed");
    }
  }
}
