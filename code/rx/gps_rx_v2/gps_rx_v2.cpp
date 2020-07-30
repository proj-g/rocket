#include <Arduino.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Adafruit_LiquidCrystal.h>

/* for feather32u4 */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0
 
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
 
// Blinky on receipt
#define LED 13
// initialize the library with the numbers of the interface pins
Adafruit_LiquidCrystal lcd(6, 5, 9, 10, 11, 12);

unsigned long time;

void display_coords(char gps_coords[50]);

void setup() {
   // set up the LCD's number of columns and rows:
  lcd.begin(20, 4);//number of columns and rows of display
  lcd.print("Starting...");
  lcd.setCursor(0,1);
  lcd.print("RX V2");

 pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600);
  delay(100);
  lcd.setCursor(0,2);
  lcd.print("INITIATE MODULE");  
    // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    lcd.print("LoRa radio init failed");
    while (1);
  }
  //Serial.println("LoRa radio init OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
   // Serial.println("setFrequency failed");
    while (1);
  }
  //Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
 
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
  lcd.setCursor(0,2);
  lcd.print("RECIEVER INITIATED");
  lcd.setCursor(0,3);
  lcd.print("WAITING FOR GPS...");
}
char buffer[55];
byte sendLen;
char timestr[8];
int loopnumber=0;

void loop()
 {
  if (rf95.available())
  {
  // lcd.print("SIGNAL ACQUIRED");
  uint8_t buf[60];
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
      if (message_type)
      {
        display_coords(buffer);
      }
      else
      {
        char* message = strtok(0, ",");
        lcd.setCursor(0,3);

        lcd.print(message);
      }
    }
  }
}

void display_coords(char gps_coords[55])
{
  //Serial.println(buf);
  char* lat= strtok(0, ",");
  char* latstr=strtok(0,",");
  char* lon=strtok(0,",");
  char* lonstr=strtok(0,",");
  char* alt=strtok(0,",");
  char* spd=strtok(0,",");
  char* bat_volt=strtok(0,",");
  char* num_sat=strtok(0,",");
  char* fix_type = strtok(0,",");
  //      Serial.println((char*) lat);
  //      Serial.println((char*) latstr);
  //      Serial.println((char*) lonstr);
  //      Serial.println(alt);

  //Latitude
  char* latdegchar= (lat);//Pointer Char type (format DDMMMM.MMMM)
  char* latminchar= (lat+2);//Exclude first two digits from char array
  float latmin= atof(latminchar);//convert min to FLOAT
  float latdegmin= atof(latdegchar);
  float latdeg=(latdegmin-latmin)/100;//Subtract MIN from DEG
  float latdegdeci=latdeg+latmin/60;//Convert MIN to deci degree and add to deg
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
  char* londegchar= lon;
  char* lonminchar=(lon+4);
  float lonmin= atof(lonminchar);
  float londegmin= atof(londegchar);
  float londeg=(londegmin-lonmin)/100;
  float londegdeci=londeg+lonmin/60;
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

  //Sats
  int int_sat = atoi(num_sat);

  //Fix
  int int_fix = atoi(fix_type);


  // Serial.print("Lon: ");
  // Serial.println(londegdeci, 5);
  // Serial.print("Alt:");
  // Serial.println(alt);
  // Serial.print("Speed: ");
  // Serial.println(spd);
  //Print to LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(latdegdeci, 5);
  lcd.print((char*) latstr+1);
  lcd.print(" ");
  //lcd.setCursor(0, 2);  
  lcd.print(londegdeci, 5);
  lcd.print((char*) lonstr+1);
  // lcd.print("W");
  lcd.setCursor(0,1);
  lcd.print("ALT: ");
  lcd.print(int_alt);
  lcd.setCursor(10,1);
  lcd.print("SPD: ");
  lcd.print(int_spd);
  lcd.setCursor(0,2);
  lcd.print("SAT ");
  lcd.print(int_sat);
  lcd.print(" FIX ");
  lcd.print(int_fix);
  lcd.print(" BAT ");
  lcd.print(d_batV);
  digitalWrite(LED, LOW);
  // delay(100);
  digitalWrite(LED, LOW);
  Serial.println(loopnumber);
  lcd.setCursor(0,3);
  lcd.print("RCV: ");
  lcd.print(rf95.lastRssi(), DEC);
  lcd.print(" dB ");
  lcd.print(loopnumber);
}