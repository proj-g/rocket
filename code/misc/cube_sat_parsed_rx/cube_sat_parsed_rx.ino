#include <SPI.h>
#include "printf.h"
#include "RF24.h"

#define CE_PIN 7
#define CSN_PIN 8
// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);

uint8_t address[][6] = { "1Node", "2Node" };
bool radioNumber = 1; // 0 uses address[0] to transmit, 1 uses address[1] to recieve

bool role = false;  // true = TX role, false = RX role
char payload[128];


void setup() {
  //Initialize Radio
  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default. Arduino doesn't have enough power in it's 3v3 pin for Max.
  Serial.print(F("radioNumber = "));
  Serial.println((int)radioNumber);
  radio.setPayloadSize(sizeof(payload));
  radio.openWritingPipe(address[radioNumber]);

  Serial.begin(115200);
  
}

void loop() {
  // This device is a RX node

    uint8_t pipe;
    if (radio.available(&pipe)) {              // is there a payload? get the pipe number that recieved it
      uint8_t bytes = radio.getPayloadSize();  // get the size of the payload
      radio.read(&payload, bytes);             // fetch payload from FIFO
      Serial.print(F("Received "));
      Serial.print(bytes);  // print the size of the payload
      Serial.print(F(" bytes on pipe "));
      Serial.print(pipe);  // print the pipe number
      Serial.print(F(": "));
      Serial.println(payload);  // print the payload's value
    }
}
