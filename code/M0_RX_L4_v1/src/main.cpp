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
// void rcv_coords(char gps_coords[50]);
void display_coords(char gps_coords[128]);
void write_display(char message[128]);

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
  tft.setTextColor(WHITE);
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

  if (rf95.available())
  {
  // tft.print("SIGNAL ACQUIRED");
  uint8_t buf[128];
  uint8_t len=sizeof(buf);
  memset(buf, '\0', sizeof(buffer));
  // Serial.print("buf:"); Serial.println((char*)buf);
  if (rf95.recv(buf, &len))
    {
      loopnumber ++;
      digitalWrite(LED, HIGH);
      //RH_RF95::printBuffer("Recieved: ", buf, len);
      Serial.print("Message Recieved: ");
      Serial.println((char*)buf); 
      char* data= (char*) buf; // (char*) is called "casting the pointer"
      
      int source_address = atof(strtok(data, ","));
      int message_type = atof(strtok(0, ","));
      Serial.println(message_type);
      if (message_type == 0x01)
      {
        display_coords(buffer);
      }
      else if (message_type >= 0x0f)
      {
        char* message = strtok(0, ",");
        tft.setCursor(0,300);

        tft.print(message);
      }
    }
    else {

    }
  }
  
}

void write_display(char message[128])
{}


void display_coords(char gps_coords[128])
{
  //Serial.println(buf);
  char* time = strtok(0, ",");
  char* latraw= strtok(0, ",");
  // char* latstr=strtok(0,",");
  char* lonraw=strtok(0,",");
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
  float lat = atof(latraw)/10000000;
  //Longitude
  float lon = atof(lonraw)/10000000;

  //spd
  int int_spd= atoi(spd)/1.944;

  //alt
  int int_alt = atoi(alt);

  //TX Volt
  double d_batV = atof(bat_volt)/1000;

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
  tft.print(lat, 5);
  // tft.print((char*) latstr+1);
  tft.print(" ");
  //tft.setCursor(0, 2);  
  tft.print(lon, 5);
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
  float oldlat = lat;
  float oldlon =lon;
  }


