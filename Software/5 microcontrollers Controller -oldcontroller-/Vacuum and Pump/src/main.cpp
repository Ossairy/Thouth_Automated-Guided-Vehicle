#include <Arduino.h>

#include <Servo.h>



Servo Vacuum;



void SetVacuum(int speed){
speed = map(speed,0,100,1000,2000);

Vacuum.writeMicroseconds(speed);


}


void setup() {
  

  Serial.begin(9600);
  

  Vacuum.attach(8);
  Vacuum.writeMicroseconds(1000);
  delay(500);


}

void loop() {

//SetVacuum(30);
Vacuum.writeMicroseconds(1000);

delay(300);

SetVacuum(15);

delay(10000);

}