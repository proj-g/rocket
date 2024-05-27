#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <SPI.h>
#include "RF24.h"
#include "printf.h"

// DEBUG Setup:
#define DEBUG 1    // SET TO 0 OUT TO REMOVE TRACES

#if DEBUG
#define D_SerialBegin(...) Serial.begin(__VA_ARGS__);
#define D_print(...)    Serial.print(__VA_ARGS__)
#define D_write(...)    Serial.write(__VA_ARGS__)
#define D_println(...)  Serial.println(__VA_ARGS__)
#else
#define D_SerialBegin(...)
#define D_print(...)
#define D_write(...)
#define D_println(...)
#endif

//GPS setup
SoftwareSerial mySerial(3, 2);
Adafruit_GPS GPS(&mySerial);
//IMU I2C Address
const int IMU = 0x68;
//Set IMU variable for reading parameters
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
//Set up timer function for checking IMU
long int lastTime = 0;
int sampleTime = 3000;
#define GPSECHO  false
#define CE_PIN 7
#define CSN_PIN 8
// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);
uint8_t address[][6] = { "1Node", "2Node" };
// Let these addresses be used for the pair
// uniquely identify which address this radio will use to transmit
bool radioNumber = 0;  // 0 uses address[0] to transmit, 1 uses address[1] to receive

// Used to control whether this node is sending or receiving
bool role = true;  // true = TX role, false = RX role
char message[128];

void  setup(){
  Serial.begin(115200);
  // D_println("Beginning Setup...");
  delay(1000);
  Wire.begin();
  Wire.beginTransmission(IMU);
  Wire.write(0x6B);  
  Wire.write(0);    
  Wire.endTransmission(true);
  GPS.begin(9600);
  
  //Initialize Radio
  D_println("Begin Radio!");
  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default. Arduino doesn't have enough power in it's 3v3 pin for Max.
  Serial.print(F("radioNumber = "));
  Serial.println((int)radioNumber);
  radio.setPayloadSize(sizeof(message));
  radio.openWritingPipe(address[radioNumber]);
  radio.stopListening();  // put radio in TX mode
  // For radio debugging info
  printf_begin();             // needed only once for printing details
  // radio.printDetails();       // (smaller) function that prints raw register values
  radio.printPrettyDetails(); // (larger) function that prints human readable data
  
  D_println("Radio Initialized");

}

void  loop()
{
  read_gps(); //We read the GPS constantly because it takes time for all the info to get pulled off
  long int currentTime = millis();
  
  if (currentTime-lastTime >= sampleTime)// After the "sample time" has elapsed we print the most recent data from both the GPS and IMU
  { 
    print_gps();
    read_accelerometer();
    lastTime = currentTime;
  } 
  // delay(1000);
}

void read_accelerometer()
{
  D_println("Reading IMU...");
  Wire.beginTransmission(IMU);
  Wire.write(0x3B);  
  Wire.endTransmission(false);
  Wire.requestFrom(IMU,12,true);  
  AcX=Wire.read()<<8|Wire.read();    
  AcY=Wire.read()<<8|Wire.read();  
  AcZ=Wire.read()<<8|Wire.read();  
  GyX=Wire.read()<<8|Wire.read();  
  GyY=Wire.read()<<8|Wire.read();  
  GyZ=Wire.read()<<8|Wire.read();  
  
  Serial.print("Accelerometer: ");
  Serial.print("X = "); Serial.print(AcX);
  Serial.print(" | Y = "); Serial.print(AcY);
  Serial.print(" | Z = ");  Serial.println(AcZ); 
  
  Serial.print("Gyroscope: ");
  Serial.print("X  = "); Serial.print(GyX);
  Serial.print(" | Y = "); Serial.print(GyY);
  Serial.print(" | Z = "); Serial.println(GyZ);
  Serial.println(" ");
}

void read_gps()
{
  char c = GPS.read();
      if ((c) && (GPSECHO))
      Serial.write(c);

    if (GPS.newNMEAreceived()) {

      if (!GPS.parse(GPS.lastNMEA()))   // Set the newNMEAreceived() flag to false
        return;  // we can fail to parse a sentence in which case we should just wait for another
  }
}

void print_gps()
{
  Serial.print("\nTime: ");
    if (GPS.hour < 10) { Serial.print('0'); }
    Serial.print(GPS.hour, DEC); Serial.print(':');
    if (GPS.minute < 10) { Serial.print('0'); }
    Serial.print(GPS.minute, DEC); Serial.print(':');
    if (GPS.seconds < 10) { Serial.print('0'); }
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    if (GPS.milliseconds < 10) {
      Serial.print("00");
    } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
      Serial.print("0");
    }
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);

  
    if (GPS.fix) {
    Serial.print("Location: ");
    Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
    Serial.print(", ");
    Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);

    Serial.print("Speed (knots): "); Serial.println(GPS.speed);
    Serial.print("Angle: "); Serial.println(GPS.angle);
    Serial.print("Altitude: "); Serial.println(GPS.altitude);
    Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
    Serial.print("Antenna status: "); Serial.println((int)GPS.antenna);
    
    sprintf(message, "%d, %d, %d, %i", GPS.day, GPS.month, GPS.year, GPS.fix );
    D_println(message);
    // send_data(message)
     bool report = radio.write(&message, sizeof(char));

    
  }
}

// void send_data(message)
// {
//   bool report = radio.write(&message, sizeof(char));
// }
