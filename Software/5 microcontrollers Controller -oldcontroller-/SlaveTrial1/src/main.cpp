#include <Arduino.h>

// Include Arduino Wire library for I2C
#include <Wire.h>
 
// Define Slave I2C Address
#define SLAVE_ADDR 9
 
// Define Slave answer size
#define ANSWERSIZE 5
 

#define PWM 6
#define DIRcw 7
#define DIRccw 8


// Define string with response to Master
String answer = "Hello";
 
 int slavecount=-1;

int parmnum=0;

int speed =0;

String MethodName ="";

bool dir=false;

int dist=0;

bool comuFlag=false;



void MoveMotor(bool dir,int speed){

Serial.println("I'm Here");
//delay(200);

}

void testMotor(){
 
 delay(2000);

 analogWrite(PWM,100);
 digitalWrite(7,HIGH);
 digitalWrite(8,LOW);
 Serial.println("hi1");

 delay(1000);
 analogWrite(PWM,0);
  digitalWrite(7,LOW);
 digitalWrite(8,LOW);
 Serial.println("hi2");
 delay(1000);
 analogWrite(PWM,100);
 digitalWrite(7,LOW);
 digitalWrite(8,HIGH);
 Serial.println("hi");
 delay(1000);
analogWrite(PWM,0);
  digitalWrite(7,LOW);
 digitalWrite(8,LOW);
 Serial.println("hif");

}




void MethodPass(){
  Serial.println("Elwinsh Fire");
  if(comuFlag){
if(MethodName.equals("Tm")){
  Serial.println("Love you Nozy");
  testMotor();
}

       else if(MethodName.equals("Ms"))
       {  
   
   Serial.println("naznoz");
   MoveMotor(dir,20);

         }
       else if(MethodName.equals("Md"))
 {

Serial.println("Adaoud");

 }  
  }
comuFlag=false;

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
    
      speed =(int)Wire.read();

  Serial.println(speed);

   }else if(slavecount==2  &&  parmnum>=2 )
    {

      dir=(bool)Wire.read();

     Serial.println(dir);
     }else if(slavecount==3  &&  parmnum>=3  ){
      
      dist = Wire.read();
         

     
     }


slavecount++;
Serial.println(slavecount);
if(slavecount==parmnum+1){
 slavecount=-1;
 //MethodPass();
}
 /*/// First Enter
   if(slavecount==-1) {
 parmnum=Wire.read();
   
   Serial.println("Hi");
  Serial.println(parmnum);
   

   
   }else if(slavecount==0 && parmnum>=0){
      
      MethodName ="";

      for(byte i=0;i<2;i++){

    char b= Wire.read();
    MethodName+=b;}
  
    
   // Serial.println(MethodName);
  

    //Serial.println("1");
    }else if(slavecount==1  &&  parmnum>=1 ){
    
      speed =Wire.read();

     

  //Serial.println(speed);

   }else if(slavecount==2  &&  parmnum>=2 ){

      dir=Wire.read();

       
     
     //Serial.println(dir);
     
     }else if(slavecount==3  &&  parmnum>=3  ){
      
      dist = Wire.read();
         
       
     
     Serial.println("4");
     
     }else{
       slavecount=-2;
        
        Serial.println("I'm inn");

     
       if(MethodName.equals("Tm")){}

       else if(MethodName.equals("Ms"))
       {  
   
   Serial.println("naznoz");
   MoveMotor(dir,20);

         }
       else if(MethodName.equals("Md"))
 {}  
       
       
 
// if()
  
     }
   
// Serial.println(MethodName);
slavecount++;
Serial.println(slavecount);
  Serial.println(parmnum);
*/
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
 
  // Initialize I2C communications as Slave
  Wire.begin(SLAVE_ADDR);
  
  // Function to run when data requested from master
  Wire.onRequest(requestEvent); 
  
  // Function to run when data received from master
  Wire.onReceive(receiveEvent);
  
  // Setup Seria

pinMode(7,OUTPUT);
pinMode(8,OUTPUT);
pinMode(PWM,OUTPUT);

   
  Serial.begin(9600);
  Serial.println("I2C Slave Demonstration");




}
 
void loop() {
 ///testMotor();

 MethodPass();
Serial.println("here");
//delay(2000);
}