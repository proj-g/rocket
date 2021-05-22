#include <Arduino.h>

int pwmpin = 12;
int ledpin = 13;
int pwmState = LOW;
int pot = A5;
const double freq = 50000; //PWM in us
double Th = 0; //High time
double Tl = 0; //Low time
double Thmax = 0.85; //limit proportion of Th
unsigned long prevTime = 0;
void setup() {
  pinMode(pwmpin, OUTPUT);
  // pinMode(A5, INPUT);
  Serial.begin(9600);

}

// the loop function runs over and over again forever
void loop() {
  unsigned long currTime = micros();
  double potvalue = analogRead(pot);
  // Serial.print("potvalue: ");
  // Serial.print(potvalue);
  Th = (potvalue/1023)*freq*Thmax;
  // Th = potvalue/1023;
  Tl= freq-Th;
  // Serial.print(" Th: ");
  // Serial.print(Th);
  // Serial.print(" Tl: ");
  // Serial.println(Tl);
  if (currTime - prevTime >= freq)
  {
    prevTime = currTime;
    if (currTime - prevTime >= Th)
    {
      prevTime = currTime;
      pwmState = LOW;
    }
    else{
      pwmState = HIGH;
    }
}
 digitalWrite(pwmpin, pwmState);
 digitalWrite(ledpin, pwmState);

}

