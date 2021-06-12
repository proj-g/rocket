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

//tft setup
#define TFT_DC 10
#define TFT_CS 9
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC);
#define SD_CS 4
#define LED 13


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
char buffer[128];
byte sendLen;
char timestr[8];
int loopnumber=0;
void display_coords(char gps_coords[50]);

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

  //tft Initialize
  tft.begin();
  tft.fillScreen(BLACK);
  tft.setRotation(1);
  tft.setTextColor(GREEN);
  tft.setCursor(1, 20);
  tft.setTextSize(2);

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
  // tft.setTextColor(RED);
  // tft.print("hello world");
  // tft.setCursor(10,10);
  // tft.setTextColor(GREEN);
  // tft.print("pig pig pig");
  // tft.setCursor(1, 20);
  // cur_time = millis();
  // tft.print(cur_time);
  if (rf95.available())
  {
  // tft.print("SIGNAL ACQUIRED");
  uint8_t buf[128];
  uint8_t len=sizeof(buf);
  memset(buf, '\0', sizeof(buffer));
  Serial.print("buf:"); Serial.println((char*)buf);
  if (rf95.recv(buf, &len))
    {
      loopnumber ++;
      digitalWrite(LED, HIGH);
      //RH_RF95::printBuffer("Recieved: ", buf, len);
      Serial.print("Message Recieved: ");
      Serial.println((char*)buf); 
      char* data= (char*) buf; // (char*) is called "casting the pointer"
      int message_type = atof(strtok(data, ","));
      Serial.println(message_type);
      if (message_type >= 1)
      {
        display_coords(buffer);
      }
      else
      {
        char* message = strtok(0, ",");
        tft.setCursor(0,3);

        tft.print(message);
      }
    }
  }
  
}

void display_coords(char gps_coords[128])
{
  //Serial.println(buf);
  char* lat= strtok(0, ",");
  // char* latstr=strtok(0,",");
  char* lon=strtok(0,",");
  // char* lonstr=strtok(0,",");
  char* alt=strtok(0,",");
  char* spd=strtok(0,",");
  char* bat_volt=strtok(0,",");
  // char* num_sat=strtok(0,",");
  // char* fix_type = strtok(0,",");
  //      Serial.println((char*) lat);
  //      Serial.println((char*) latstr);
  //      Serial.println((char*) lonstr);
  //      Serial.println(alt);

  //Latitude

  float latdeci = atof(lat)/10000000;
  // char* latdegchar= (lat);//Pointer Char type (format DDMMMM.MMMM)
  // char* latminchar= (lat+3);//Exclude first two digits from char array
  // float latmin= atof(latminchar);//convert min to FLOAT
  // float latdegmin= atof(latdegchar);
  // float latdeg=(latdegmin-latmin)/100;//Subtract MIN from DEG
  // float latdegdeci=latdeg+latmin/60;//Convert MIN to deci degree and add to deg
  // Serial.print("Lat: ");
  // Serial.println(latdegdeci, 5);
  // Serial.print("lat: ");
  // Serial.println(lat);
  // Serial.print("latminchar: ");
  // Serial.println(latminchar);
  // Serial.print("latdegmin: ");
  // Serial.println(latdegmin);
  // Serial.print("latdegchar: ");
  // Serial.println(latdegchar);


  //Longitude
  float londeci = atof(lon)/10000000;
  // char* londegchar= lon;
  // char* lonminchar=(lon+4);
  // float lonmin= atof(lonminchar);
  // float londegmin= atof(londegchar);
  // float londeg=(londegmin-lonmin)/100;
  // float londegdeci=londeg+lonmin/60;
  // Serial.print("lon: ");
  // Serial.println(lon);
  // Serial.print("lonminchar: ");
  // Serial.println(lonminchar);
  // Serial.print("londegmin: ");
  // Serial.println(londegmin);
  // Serial.print("londeg: ");
  // Serial.println(londeg);
  // Serial.print("lonmin: ");
  // Serial.println(lonmin);

  //spd
  int int_spd= atoi(spd)/1.944;

  //alt
  int int_alt = atoi(alt);

  //TX Volt
  double d_batV = atof(bat_volt);

  // //Sats
  // int int_sat = atoi(num_sat);

  // //Fix
  // int int_fix = atoi(fix_type);
int screen_width = 480;
int screen_height = 320;
int col1 = 0;
int col2 = 240;
int line1 = 0;
int line2 = 20;
int line3 = 40;
int line4 = 60;

  // Serial.print("Lon: ");
  // Serial.println(londegdeci, 5);
  // Serial.print("Alt:");
  // Serial.println(alt);
  // Serial.print("Speed: ");
  // Serial.println(spd);
  //Print to tft
  // tft.fillScreen(BLACK);
  tft.fillRect(col1, line1, col2-col1, line2-line1, BLACK);
  tft.setCursor(line1, col1);
  tft.print(latdeci, 5);
  // tft.print((char*) latstr+1);
  tft.print(" ");
  //tft.setCursor(0, 2);  
  tft.print(londeci, 5);
  // tft.print((char*) lonstr+1);
  // tft.print("W");
  tft.fillRect(col2, line1, screen_width-col2, line4-line1, BLACK);
  tft.setCursor(col2,line1);
  tft.print("ALT: ");
  tft.print(int_alt);
  tft.print(" ");
  // tft.setCursor(10,0);
  tft.print("SPD: ");
  tft.println(int_spd);
  tft.setCursor(col2,line2);
  // tft.print("SAT ");
  // tft.print(int_sat);
  // tft.print(" FIX ");
  // tft.print(int_fix);
  tft.print("BAT ");
  tft.print(d_batV);
  digitalWrite(LED, LOW);
  // delay(100);
  digitalWrite(LED, LOW);
  Serial.println(loopnumber);
  tft.setCursor(col2,line3);
  tft.print("RCV: ");
  tft.print(rf95.lastRssi(), DEC);
  tft.print(" dB ");
  tft.print(loopnumber);
  float oldlat = latdeci;
  float oldlon =londeci;
}


