// This code works to repeate messages from teh tx.
// Current issue (apr 2024), if TX message is incomplete EG all zeros, the buffer length (len) is set too short
// and misses the ending characters. Forcing len to be a constant cause the code to only send the first two characters

#include <Arduino.h>
#include <RH_RF95.h>
#include <SPI.h>

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define RF95_FREQ 903.0
#define rx_led 13
#define tx_led 11
// #define DEBUG

// #ifdef DEBUG
//   #define DEBUG_PRINT(x)  Serial.print (x)
//   #define DEBUG_PRINTLN(x)  Serial.println (x)
// #else
//   #define DEBUG_PRINT(x)
// #endif

RH_RF95 rf95(RFM95_CS, RFM95_INT);

const int buf_len = 88;
char buffer[buf_len];
float txFreq = 903.0;
void send_data(char message [buf_len]);
void r_data(uint8_t* message, uint8_t mess_len);
// void receive_data(char incoming [128]);
float check_bat ();
static int source_address = 0x0f;
uint8_t buf[buf_len];
uint8_t len=sizeof(buf);//must be size of the associated buffer
// uint8_t relay_mess[88];
char relay_mess[buf_len];
int send_delay = 500;
unsigned long time;
unsigned long rx_time;
bool new_mess = false;


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
  
  
  digitalWrite(tx_led, HIGH);
  digitalWrite(rx_led, HIGH);
  delay(1000);
  digitalWrite(tx_led, LOW);
  digitalWrite(rx_led, LOW);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  time = millis();
  memset(buf, '\0', sizeof(buf));
  len = buf_len;
  // DEBUG_PRINT("length of Buffer" );
  // DEBUG_PRINTLN(buf);
  if(rf95.recv(buf, &len))
  {
  r_data(buf, len);
  rx_time = time;
  new_mess = true;
  // Serial.print("Len is: ");
  // Serial.println(len);
  //Change the buf message
  String str_buf = (char*)buf;
  String buf_sans_address = str_buf.substring(4); //Reads string starting at 4th character
  char * bsa_char;//Create a character array to convert string to char
  bsa_char = &buf_sans_address[0]; //Here's where the string actually becomes type char
  sprintf(relay_mess, "%i, %s", source_address, bsa_char); //print it as a new message

  // sprintf(new_message, "%i, " )
  // strcpy((char*)relay_mess,  )
  }
  // Serial.print("buf: ");
  // Serial.println((char*)buf);
  
  if (time-rx_time >= send_delay && new_mess == true) 
  {
    send_data(relay_mess);
    new_mess = false;
    memset(relay_mess, '\0', sizeof(relay_mess));
  //     Serial.print("relay_mess: ");
  // Serial.println(relay_mess);
  }
}

void r_data(uint8_t* message, uint8_t mess_len)
{
    // receive_data();
    digitalWrite(rx_led, HIGH);
    // Serial.print("Message recieved: ");
    Serial.println((char*)message); //buf is global variable, so buf or message work here
    // Serial.print("RSSI: ");
    // Serial.println(rf95.lastRssi(), DEC);
    digitalWrite(rx_led, LOW);

}

void send_data(char message [buf_len])
{
  digitalWrite(tx_led, HIGH);
  byte sendLen;
  sendLen = strlen(message);
  rf95.send((uint8_t*)message, sendLen);
  // Serial.print("Sending: "); 
  Serial.println(message);
  digitalWrite(tx_led, LOW);
  delay(50);
}