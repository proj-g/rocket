#include <Arduino.h>
#include <RH_RF95.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_HX8357.h>
// #include <RingBufCPP.h>

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
#define MAX_NUM_ELEMENTS 10


// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

// struct ringbuf
// {
//   int index;
//   int time_stamp;
//   float latitude;
//   float longitude;
// };

// RingBufCPP<struct ringbuf, MAX_NUM_ELEMENTS> buf;
// volatile unsigned int index = 0;
unsigned long cur_time = 0;
char buffer[128];
byte sendLen;
char timestr[8];
int loopnumber=0;
int source_address = 0;
float oldlat = 0;
float oldlon = 0;
float latlist[10];
float lonlist[10];
int timelist[10];
int hist[10][3];
int time_hist[10];
int i = 0;
int recv_data [8];
bool set_time = false;
int start_time = 0;
int rcv_time = 0;


// void rcv_coords(char gps_coords[50]);
void display_coords(char gps_coords[128]);
void write_display(char message[128]);
void print_array(int array[10][3]);
void print_buf_contents();

void parse_recieve(char* message_data)
{
  int i = 0;
  for (char* p = strtok(message_data,","); p!= NULL; p = strtok(NULL,","))
  {
    Serial.println(p);
    recv_data[i]=atoi(p);
    i++;
  }
}

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
  
  set_time = true;
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
      // Serial.print("Message Recieved: ");
      Serial.print((char*)buf);
      Serial.print(", ");
      Serial.println(rf95.lastRssi(), DEC);
      char* data= (char*) buf; // (char*) is called "casting the pointer"
      // parse_recieve((char*)buf);
      // Serial.print("Recv_data: ");
      
      // for(int i =0; i >= 8; i++)
      // {
      // Serial.println(recv_data[i]);
      // }
      
      source_address = atof(strtok(data, ","));
      int message_type = atof(strtok(0, ","));
      // Serial.println(message_type);
      // // Serial.print("Buffer: ");
      // // Serial.println(buffer);
      if (message_type == 0x01)//0x01 is nominal coord message
      {
        display_coords(data);
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

void print_array(int array[10][3])
{
 //find number of array elements
//  int array_size= sizeof(array)/sizeof(float);
//  Serial.println(array_size);
// Serial.println("Print Array: ");
// Serial.println(array[0][0]);
// Serial.print("Array end: ");
// Serial.println(array[9][9]);

  // for (int j=0; j>= 10; j++)
  // {
  //  for (int k=0; k>=3; k++){
  //   Serial.print(array[j][k]);
  // }
  // Serial.println();
// }
}

void display_coords(char gps_coords[128])
{
  //Serial.println(buf);
  char* timeraw = strtok(0, ",");
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
  //Time
  int time = atoi(timeraw);
  // Serial.print("Time: ");
  // Serial.println(time);
  //Latitude
  float lat = atof(latraw)/10000000;
  // int lat_int=atoi(latraw);
  //Longitude
  float lon = atof(lonraw)/10000000;
  // int lon_int=atoi(lonraw);
  //spd
  int int_spd= atoi(spd)/1.944;
  //alt
  int int_alt = atoi(alt);
  //TX Volt
  double d_batV = atof(bat_volt)/1000;

  if( set_time == true)
  {
    start_time = time;
    set_time = false;
  }
  else
  {
    rcv_time = time - start_time;
  }
  // //Sats
  // int int_sat = atoi(num_sat);
  // //Fix
  // int int_fix = atoi(fix_type);
  
  // if(lat - oldlat >= abs(0.0001) || lon-oldlon >= abs(0.0001)){
  // for(int i = 0; i< 10; i++){
    
    // struct ringbuf gps_data;
    // // gps_data.index= index++;
    // gps_data.latitude = lat;
    // gps_data.longitude = lon;
    // gps_data.time_stamp = time;
    // buf.add(gps_data);
    // print_buf_contents();
    //   latlist[i] = lat;
    //   lonlist[i] = lon;
    //   timelist[i] = time;
    // hist[i][0]= time;
    // hist[i][1]= lat_int;
    // hist[i][2]= lon_int;
    // Serial.print(hist[0][0]);
    // Serial.print(hist[0][1]);
    // Serial.println(hist[0][2]);
    // Serial.print(hist[1][0]);
    // Serial.print(hist[1][1]);
    // Serial.println(hist[1][2]);
    // Serial.print(hist[2][0]);
    // Serial.print(hist[2][1]);
    // Serial.println(hist[2][2]);
    // Serial.println(hist[1][0]);
    // Serial.println(hist[2][0]);
  //   Serial.print("i= ");
  //   Serial.println(i);
  //   i++;
  //   if (i>9){
  //     i=0;
  //     // print_array(hist);
  //     }
  //   // print_array(hist);
  //   // }


  // // }




  int screen_width = 480;
  int screen_height = 320;
  int col1 = 0;
  int col2 = 240;
  int line1 = 0;
  int line2 = 20;
  int line3 = 40;
  int line4 = 60;
  int line5 = 80;
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
  tft.fillRect(col2, line1, screen_width-col2, line5-line1, BLACK);
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
  tft.print(" ");
  tft.print("SRC: ");
  tft.print(source_address);
  digitalWrite(LED, LOW);
  // delay(100);
  // digitalWrite(LED, LOW);
  // Serial.println(loopnumber);
  tft.setCursor(col2,line3);
  tft.print("RCV: ");
  tft.print(rf95.lastRssi(), DEC);
  tft.print(" dB ");
  tft.print(loopnumber);
  // tft.setCursor(col2,line4);
  
  if(source_address == 10)
  {
  tft.fillRect(col2, line4, screen_width-col2, line5-line4, MAGENTA);

  }
  if(source_address == 15)
  {
  tft.fillRect(col2, line4, screen_width-col2, line5-line4, CYAN);
  }
  tft.setCursor(col2+5, line4+2);
  tft.setTextColor(BLACK);
  tft.print("TIME: ");
  tft.print(rcv_time);  
  tft.setTextColor(WHITE);
  // oldlat = lat;
  // Serial.print("Oldlat: ");
  // Serial.println(oldlat, 5);

  // if (lat != 0 && lon !=0)
  // {
  //   float oldlat = lat;
  //   float oldlon = lon;
  // }
  // else
  // {
  //   tft.fillRect(col1, line2, col2-col1, line3-line2, BLACK);
  //   tft.setTextColor(RED);
  //   tft.setCursor(col1, line2);
  //   tft.print(oldlat, 5);
  //   tft.print(" ");
  //   tft.print(oldlon, 5);
  //   tft.setTextColor(WHITE);
  // }
}

// void print_buf_contents()
// {
//   struct ringbuf gps_data;

//   Serial.println("\n______Dumping contents of ring buffer_______");

//   // Keep looping until pull() returns NULL
//   while (buf.pull(&gps_data))
//   {
//     //
//     Serial.print("index: ");
//     Serial.println(gps_data.index);

//     Serial.print("Latitude: ");
//     Serial.println(gps_data.latitude);

//     Serial.print("Longitude: ");
//     Serial.println(gps_data.longitude);
    
//     Serial.print("Timestamp: ");
//     Serial.println(gps_data.time_stamp);

//     Serial.println();
//   }

//   Serial.println("______Done dumping contents_______");

// }
