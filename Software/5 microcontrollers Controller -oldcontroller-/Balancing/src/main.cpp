#include <Arduino.h>


#include "Wire.h"
#include <MPU6050_light.h>

MPU6050 mpu(Wire);
unsigned long timer = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(); // gyro and accelero
  Serial.println("Done!\n");
}

void loop() {
  mpu.update();
  
int x =mpu.getGyroX();
int y =mpu.getAngleY();
int z = mpu.getAngleZ();

  if((millis()-timer)>10){ // print data every 10ms
	Serial.print("X : ");
	Serial.print(x);
	Serial.print("\tY : ");
	Serial.print(y);
	Serial.print("\tZ : ");
	Serial.println(z);
	timer = millis();  
  
  
  }
}