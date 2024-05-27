#include <Adafruit_PMTK.h>
#include <Adafruit_GPS.h>
#include <NMEA_data.h>
#include <SoftwareSerial.h>
#include <Wire.h>
long int lastTime = 0;
int sampleGyro = 1000;

//GPS setup
SoftwareSerial mySerial(7, 3);
#define PMTK_SET_NMEA_UPDATE_1HZ  "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_5HZ  "$PMTK220,200*2C"
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F"
// turn on only the second sentence (GPRMC)
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
// turn on GPRMC and GGA
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn on ALL THE DATA
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn off output
#define PMTK_SET_NMEA_OUTPUT_OFF "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
#define PMTK_Q_RELEASE "$PMTK605*31"



const int IMU = 0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

void  setup(){
  Wire.begin();
  Wire.beginTransmission(IMU);
  Wire.write(0x6B);  
  Wire.write(0);    
  Wire.endTransmission(true);
  Serial.begin(9600);
  mySerial.begin(9600);


    Serial.println("Software Serial GPS Test Echo Test");
  // you can send various commands to get it started
  //mySerial.println(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  mySerial.println(PMTK_SET_NMEA_OUTPUT_RMCGGA);

  mySerial.println(PMTK_SET_NMEA_UPDATE_1HZ);

  Serial.println("Get version!");
  mySerial.println(PMTK_Q_RELEASE);

}

void  loop(){
  long int currentTime = millis();
    if (currentTime-lastTime >= sampleGyro)
  { 
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
  lastTime = currentTime;
  }
  
  
  if(Serial.available()) {
   char c = Serial.read();
   Serial.write(c);
   mySerial.write(c);
  }
  if (mySerial.available()) {
    char c = mySerial.read();
    Serial.write(c);
  }
  
  
//  delay(100);
}
