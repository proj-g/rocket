#include <Arduino.h>
#include <RH_RF95.h>
#include <Adafruit_GPS.h>
#include <Adafruit_SleepyDog.h>


// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}