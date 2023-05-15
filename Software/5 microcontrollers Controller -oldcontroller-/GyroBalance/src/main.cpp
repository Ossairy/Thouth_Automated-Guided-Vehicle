#include <Arduino.h>

#include "Wire.h"
#include <MPU6050_light.h>

MPU6050 mpu(Wire);
long timer = 0;



volatile double Kp = 1.2367 ;   // the constant tunings for the PID
volatile double Ki = 1.5736 ; 
volatile double Kd = 0.058  ;


volatile double integral = 0  ;   // initialize variables of PID
volatile double derivative = 0 ;
volatile double error = 0 ;
volatile double lastError = 0 ;
volatile double correction= 0;


void Move(int address ,int speed,bool dirction){

Serial.print("Motor   ");
Serial.print(address);
Serial.print("    Speed");
Serial.print(speed);
Serial.println();
Serial.println(F("=====================================================\n"));
delay(10);



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

int powerB = speed  - correction ;
int powerA = speed + correction ; 

if (error < 2 & error > -2){
    powerA=speed ;
    powerB=speed  ; 
   }

if (powerA > 35)
    powerA = 35;
if (powerB > 35)
    powerB = 35;

if (powerA >0 ){
    Move(1,powerA,1);
    }
  else if(powerA <0){
    powerA= 0;
    Move(1,powerA,0);
    }
  if (powerB >0){
    Move(2,powerB,1);
    }
  else{
    powerB= 0 ;
    Move(2,powerB,0);
    }


delay(10);




}





void setup() {
  
  Serial.begin(9600);
  Wire.begin();
  
  
  
  
  /////////---------------------GyroScope------------------------------///////////

    byte status = mpu.begin();
    while(status!=0){ }
    delay(1000);
    mpu.calcOffsets(true,true); 
  /////////---------------------------------------------------///////////

}

void loop() {
  
  
  
  
Balance(20 , 1, 0 );
  
  
 // mpu.update();

   // Serial.print("\tZ: ");Serial.println((int)mpu.getAngleZ());
   // Serial.println(F("=====================================================\n"));



}