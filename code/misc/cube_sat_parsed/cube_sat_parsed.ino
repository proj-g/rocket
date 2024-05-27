#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <Wire.h>

//GPS setup
SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);
//IMU I2C Address
const int IMU = 0x68;
//Set IMU variable for reading parameters
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
//Set up timer function for checking IMU
long int lastTime = 0;
int sampleTime = 3000;
#define GPSECHO  false

void  setup(){
  Wire.begin();
  Wire.beginTransmission(IMU);
  Wire.write(0x6B);  
  Wire.write(0);    
  Wire.endTransmission(true);
  Serial.begin(115200);
  GPS.begin(9600);

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
  }
}
