#include <Arduino.h>

bool nano =true;
 
// Include Arduino Wire library for I2C
#include <Wire.h>
 

#include <MPU6050_light.h>

MPU6050 mpu(Wire);
long timer = 0;



volatile double Kp = 0.5 ;   // the constant tunings for the PID
volatile double Ki = 0 ; 
volatile double Kd = 0 ;


volatile double integral = 0  ;   // initialize variables of PID
volatile double derivative = 0 ;
volatile double error = 0 ;
volatile double lastError = 0 ;
volatile double correction= 0;



// Define Slave I2C Address
#define BLACK_SHOULDER 10
#define WHITE_SHOULDER 9

 
// Define Slave answer size
#define ANSWERSIZE 5




unsigned long Timer=0;










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
 



void Balance(int speed , bool direction , int targetangle ){


mpu.update();



int error = targetangle-mpu.getAngleZ();

   if (error == 0){
       integral = 0 ;
   }
   
   else{
    integral = integral + error ;
    derivative = error - lastError  ;
    }

correction = ((Kp*(error)) + (Ki*(integral)) + (Kd*(derivative))) * -1;

int powerA = speed  - correction ;
int powerB = speed + correction ; 

if (error < 2 & error > -2){
    powerA=speed ;
    powerB=speed  ; 
   }

if (powerA > 35)
    powerA = 35;
if (powerB > 35)
    powerB = 35;

if (powerA >0 ){
    Move(BLACK_SHOULDER,1,powerA);
    }
  else if(powerA <0){
    //powerA= 0;
   // Move(BLACK_SHOULDER,0,powerA);
    }
  if (powerB >0){
    Move(WHITE_SHOULDER,1,powerB);
    }
  else if (powerB<0){
    //powerB= 0 ;
   // Move(WHITE_SHOULDER,0,powerB);
    }

Serial.print("Motor 1 Speed :  ");
Serial.print(powerA);
Serial.print("   Motor 2 Speed :  ");
Serial.print(powerB);
Serial.println();


Timer=millis();



//elay(100);




}










void setup() {
 
  // Initialize I2C communications as Master
  Wire.begin();
  

 
  
  




  // Setup serial monitor
  Serial.begin(9600);
  Serial.println("I2C Master Demonstration");


delay(200);






  
}
 
void loop() {

//testMotor(WHITE_SHOULDER);
//testMotor(BLACK_SHOULDER);
//Move(SLAVE_ADDR,true,10);
/*
Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(0);
  Wire.endTransmission();

  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write("Tm");
  Wire.endTransmission();

*/

//Move(BLACK_SHOULDER,1,30);
//Move(WHITE_SHOULDER,1,20);
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

//Balance(20 , 1, 0 );

delay(5000);


/*
  String response = "";
  while (Wire.available()) {
      char b = Wire.read();
      response += b;
  } 
  
  // Print to Serial Monitor
  Serial.println(response);

*/

}


