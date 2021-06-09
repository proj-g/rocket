#include <Arduino.h>
#include <RH_RF95.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_HX8357.h>

//RFM95 Setup
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
// #define RF95_FREQ 903.0
RH_RF95 rf95(RFM95_CS, RFM95_INT);
float Freq = 903.0;

//LCD setup
#define TFT_DC 10
#define TFT_CS 9
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC);

#define SD_CS 4


// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF




unsigned long cur_time = 0;

void setup() {
  Serial.begin(115200);
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

  //LCD Initialize
  tft.begin();
  tft.fillScreen(BLACK);
  tft.setRotation(1);

  // void setCursor(uint16_t 0, uint16_t 0);
  // void setTextColor(BLUE);
  // // void setTextColor(uint16_t color, uint16_t BLACK);
  // void setTextSize(uint8_t 1);
  // void setTextWrap(boolean w);


}

void loop() {
  tft.setCursor(1, 1);
  // void setCursor(uint16_t x0, uint16_t y0);
  // void setTextColor(RED);
  tft.setTextColor(RED);
  tft.print("hello world");
  tft.setCursor(10,10);
  tft.setTextColor(GREEN);
  tft.print("pig pig pig");
  tft.setCursor(1, 20);
  cur_time = millis();
  tft.print(cur_time);
  

  
}



