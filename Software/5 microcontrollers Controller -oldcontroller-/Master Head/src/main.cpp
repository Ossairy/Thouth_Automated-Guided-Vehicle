#include <Arduino.h>

bool nano =true;
 
// Include Arduino Wire library for I2C
#include <Wire.h>
 
// Define Slave I2C Address
#define BLACK_SHOULDER 10
#define WHITE_SHOULDER 9

 
// Define Slave answer size
#define ANSWERSIZE 5






void testMotor(int slaveadd){

Wire.beginTransmission(slaveadd);
  Wire.write(0);
  Wire.endTransmission();

  Wire.beginTransmission(slaveadd);
  Wire.write("Tm");
  Wire.endTransmission();


}

void Move(int slaveadd , bool dirction , int speed ){


Wire.beginTransmission(slaveadd);
  Wire.write(2);
  Wire.endTransmission();

  Wire.beginTransmission(slaveadd);
  Wire.write("Ms");
  Wire.endTransmission();

  Wire.beginTransmission(slaveadd);
  Wire.write(speed);
  Wire.endTransmission();

  Wire.beginTransmission(slaveadd);
  Wire.write(dirction);
  Wire.endTransmission();

  



}
void Move(int slaveadd , bool dirction , int speed ,int distance){



Wire.beginTransmission(slaveadd);
  Wire.write(3);
  Wire.endTransmission();

  Wire.beginTransmission(slaveadd);
  Wire.write("Md");
  Wire.endTransmission();

  Wire.beginTransmission(slaveadd);
  Wire.write(speed);
  Wire.endTransmission();

  Wire.beginTransmission(slaveadd);
  Wire.write(dirction);
  Wire.endTransmission();

  Wire.beginTransmission(slaveadd);
  Wire.write(distance);
  Wire.endTransmission(); 
  


}

void Readvelocity(int slaveadd){

}
 
void setup() {
 
  // Initialize I2C communications as Master
  Wire.begin();
  
  // Setup serial monitor
  Serial.begin(9600);
  Serial.println("I2C Master Demonstration");


delay(2000);






  
}
 
void loop() {

//testMotor(SLAVE_ADDR);

//Move(SLAVE_ADDR,true,10);
/*
Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(0);
  Wire.endTransmission();

  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write("Tm");
  Wire.endTransmission();

*/

Move(BLACK_SHOULDER,1,20);
Move(WHITE_SHOULDER,1,20);
/*

Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(3);
  Wire.endTransmission();

  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write("Md");
  Wire.endTransmission();

  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(10);
  Wire.endTransmission();

  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(1);
  Wire.endTransmission();

  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(1);
  Wire.endTransmission(); 
  


*/




  delay(5000);



  String response = "";
  while (Wire.available()) {
      char b = Wire.read();
      response += b;
  } 
  
  // Print to Serial Monitor
  Serial.println(response);



}


