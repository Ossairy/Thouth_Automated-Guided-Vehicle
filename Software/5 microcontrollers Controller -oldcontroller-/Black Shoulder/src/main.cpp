#include <Arduino.h>


#include <Wire.h>

////////////////////////////////////////////////////////////////////



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
int BlncError=0;


/////////////////////////////////////////////////////////////////




#define ANSWERSIZE 5
#define SLAVE_ADDR 10

int slavecount=-1;
int parmnum=0;
int slavespeed =0;
String MethodName ="";
bool dir=false;
int dist=0;
String answer = "Hello";


#define ENC_COUNT_REV 180
#define ENC_IN1 3
#define ENC_IN2 5
#define PWM 6
#define DIRcw 7
#define DIRccw 8

int speedcontrol =0 ;
volatile long encoderValue =0 ;
int interval = 1000;
long previousMillis =0;
long currentMillis =0;
int rpm=0;
int motorPWM =0;

int prevpos=0;
int i =0;
int v =0;
//MOTOR SPEED PID CONTROLLER

 //int pos = 0;
 volatile long prevT_i = 0;
 float eprev = 0; 
 float eintegral = 0;
 volatile int pos_i = 0;
 volatile float velocity_i = 0;
  volatile long prevT2 = 0;
//MOTOR SPEED PID CONTROLLER
bool Dang;

bool nano=true;

bool comuFlag=true;


void updateEncoder(){
  //encoderValue++;
pos_i++;

long currT = micros();
float deltaT = ((float)(currT - prevT_i))/1.0e6;
velocity_i = 1/deltaT;
prevT_i = currT;

 // Serial.println("I'm Here");
}

void EMERGENCYSTOP(){

 digitalWrite(DIRccw,LOW);

 digitalWrite(DIRcw,LOW);
 Dang=true;
 while(Dang){

if(digitalRead(9))
 Dang=false;

 };

}



void Move( int speed , bool direction ){



int e = speed-fabs(v);
// Delay for soft Start
//delay(10);
//int count=0;
Serial.print(e);
//while(fabs(e)>1 ){
int target = speed;
//count++;
float kp =2;
float kd = 0.01;
float ki =7;

//Time difference
long currT2 =micros();
float deltaT = ((float)(currT2-prevT2))/1.0e6;
prevT2 = currT2 ;

v = velocity_i/180*60;

 

 e = target-v;

//derivative
float dedt = (e-eprev)/(deltaT);
//Integral
eintegral = eintegral + e*deltaT;
// Control Signal
float u = kp*e + kd*dedt + ki*eintegral ;

Serial.print("   ");
Serial.print(u);

if(direction){



if(u>0){


if(u>255){
 u=255;
eintegral=0;


}
Serial.print("cw");
Serial.print("    ");
Serial.print(u);
digitalWrite(DIRcw, HIGH);
  digitalWrite(DIRccw, LOW);
}else if(u<0){
  
digitalWrite(DIRcw, LOW);
  digitalWrite(DIRccw, HIGH);


  if(u<-255){
  u=0;
eintegral=0;

  //digitalWrite(DIRcw, HIGH);
  //digitalWrite(DIRccw, LOW);

   }

  Serial.print("ccw");
Serial.print("    ");
Serial.print(u);



}
  
}else{


if(u<0){


digitalWrite(DIRcw, HIGH);
  digitalWrite(DIRccw, LOW);

if(u<-255)
  u=-255;


}else if(u>0){

digitalWrite(DIRcw, LOW);
  digitalWrite(DIRccw, HIGH);

if(u>255)
 u=255;

}

}



motorPWM = (int)fabs(u);
if(motorPWM>250)
 motorPWM =250;

analogWrite(PWM,motorPWM);

if(pos_i==prevpos){

if(i>15) {
  velocity_i = 0;
  v=0;
  i =0;
   }
   i++;
}
prevpos = pos_i ;

//Serial.print(velocity_i);
//nano=false;
//Serial.println();
e=eprev;

comuFlag=true;


}

//}





void ReadVelocity(){

if(pos_i==prevpos){

if(i>15) {
  velocity_i = 0;
  v=0;
  i =0;
   }
   i++;
}
prevpos = pos_i ;


//Serial.print(pos_i);
Serial.print(" SPEED ");
Serial.print(v*0.0066497045);
Serial.print(" M/S ");
//Serial.print(motorPWM);
Serial.print("  ");
Serial.print(" SPEED: ");
Serial.print(v);
Serial.println(" RPM");

}


void Move(int speed , bool direction , int distance){
  
  pos_i = 0 ;
  
  int targetpos = distance*454.7284;
  while(pos_i< targetpos){

   Move(speed,direction);
     Serial.println(motorPWM);
  }
  Serial.print("i'm out");
  Move(0,false);
  //volatile  estimatedtime = millis + ((distance)/(v*0.0066497045));
  ///while(millis()<=estimatedtime){
    //Move(speed,direction);
   //Serial.print("i'm still here");
   // Serial.print("   ");
    //Serial.print(millis());
   // Serial.print("   ");
    //Serial.print(estimatedtime);
    //Serial.println();  
 // }
 //EMERGENCYSTOP();
   //Move(0,false);
   comuFlag=false;
}


void testMotor(){

//delay(2000);

//Move Forward for only single motor

Move(15,HIGH,1);

delay(1000);

  Move(0,LOW);

delay(1000);

//Move Backward 

Move(15,LOW,1);

delay(1000);


Move(0,LOW);

delay(1000);


//spen
//pos_i=0;
comuFlag=false;

}







void Balance(int speed , bool direction , int targetangle ){


mpu.update();


int power=speed;
int error = targetangle-mpu.getAngleZ();

BlncError=error;

   if (error == 0){
       integral = 0 ;
   }
   
   else{
    integral = integral + error ;
    derivative = error - lastError  ;
    }

correction = ((Kp*(error)) + (Ki*(integral)) + (Kd*(derivative))) ;
if(error>0)
 power = speed  - correction ;
else if(error<0)
 power = speed  - correction ;

if (error < 2 & error > -2){
    power=speed ;
    
   }

if (power > 35)
    power = 35;

if (power>0 ){
    Move(power,direction);
    }

else if(power<0){
    Move(power,(direction) ^ 1);
}


//Serial.print("Motor 1 Speed :  ");
//Serial.print(power);

//Serial.println();


//imer=millis();





}





void Balance(int speed,bool direction ,int angel,float distance){

pos_i = 0 ;
  
  float targetpos = distance*454.7284;
while(pos_i< targetpos || fabs(BlncError)>1){
  
  
  
  
Balance(speed,direction,angel);





  }
  Serial.print("i'm out");
  Move(0,false);


}





void MethodPass(){
  //Serial.println("Elwinsh Fire");
 
  if(comuFlag){
if(MethodName.equals("Tm")){

    testMotor();
Serial.println("naznoz & Adaoud");

}

       else if(MethodName.equals("Ms"))
       {  
   
   //Serial.println("naznoz");
   Move(slavespeed,dir);

         }
       else if(MethodName.equals("Md"))
 {

   Move(slavespeed,dir,dist);
//Serial.println("Adaoud");

 }  

}
//comuFlag=false;

}

void receiveEvent() {
 

comuFlag=true;

if(slavecount==-1) {
 parmnum=Wire.read();
   
   Serial.println("Hi");
  Serial.println(parmnum);}

else if(slavecount==0 && parmnum>=0){
      
      MethodName ="";

      for(byte i=0;i<2;i++){
    char b= Wire.read();
    MethodName+=b;
    }  
   Serial.println(MethodName);
    }else if(slavecount==1  &&  parmnum>=1 ){
    
      slavespeed =(int)Wire.read();

  Serial.println(slavespeed);

   }else if(slavecount==2  &&  parmnum>=2 )
    {

      dir=(bool)Wire.read();

     Serial.println(dir);
     }else if(slavecount==3  &&  parmnum>=3  ){
      
      dist =(int)Wire.read();
         

     
     }


slavecount++;
//Serial.println(slavecount);
if(slavecount==parmnum+1){
 slavecount=-1;
 //MethodPass();
}
 
}


void requestEvent() {
 
  // Setup byte variable in the correct size
  byte response[ANSWERSIZE];
  
  // Format answer as array
  for (byte i=0;i<ANSWERSIZE;i++) {
    response[i] = (byte)answer.charAt(i);
  }
  
  // Send response back to Master
  Wire.write(response,sizeof(response));
  
  // Print to Serial Monitor
 /// Serial.println("Request event");
}
 






void setup() {


 Wire.begin();
// delay(1000);
Serial.begin(9600);

Serial.println("I'm Inn");
  /////////---------------------GyroScope------------------------------///////////

   byte status = mpu.begin();
    while(status!=0){ }
    delay(1000);
    mpu.calcOffsets(true,true); 
  /////////---------------------------------------------------///////////


Serial.println("I'm OUT");

delay(2000);



pinMode(A0,INPUT);

pinMode(ENC_IN1, INPUT_PULLUP);
pinMode(ENC_IN2, INPUT_PULLUP);

pinMode(PWM,OUTPUT);

pinMode(DIRcw , OUTPUT);
pinMode(DIRccw, OUTPUT);

pinMode(2,INPUT_PULLUP);

//testMotor();
Serial.println("hi");


attachInterrupt(digitalPinToInterrupt(3),updateEncoder , RISING);

attachInterrupt(digitalPinToInterrupt(2), EMERGENCYSTOP, RISING);

previousMillis = millis();


Serial.println("hi");
// Initialize I2C communications as Slave
Wire.begin(SLAVE_ADDR);
  
  // Function to run when data requested from master
  Wire.onRequest(requestEvent); 
  
  // Function to run when data received from master
  Wire.onReceive(receiveEvent);




Serial.begin(9600);


}





void loop(){


//Serial.println("hi");

//MethodPass();

if(digitalRead(A0))

Balance(20,1,0);


else
  Move(0,0);

//Serial.println("hi");

//delay(15);

//Balance(20,1,0,1);

//Serial.println("hi");

//delay(10000);

//Move(20,1);

}