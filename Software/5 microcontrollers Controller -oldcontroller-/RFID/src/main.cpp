#include <Arduino.h>
#include <AccelStepper.h>
#include <SPI.h>
/* Include the RFID library */
#include <MFRC522.h>
#include <Servo.h>

bool firsttime=1;

#include <Wire.h>

bool LED=0;

#define SDA_DIO 53
#define RESET_DIO 31
/* Create an instance of the RFID library */
MFRC522 mfrc522(SDA_DIO, RESET_DIO); 

  String content= "";


#define motorPin1  8     
#define motorPin2  9    
#define motorPin3  10   
#define motorPin4  11    
AccelStepper stepper1(8, motorPin1, motorPin3, motorPin2, motorPin4);



Servo Vacuum;



unsigned long long Timer=0;


void SetVacuum(int speed){

Vacuum.writeMicroseconds(1000);

if(firsttime)
{delay(300);
 firsttime=0;
}

speed = map(speed,0,100,1000,2000);

Vacuum.writeMicroseconds(speed);


}








void RFIDRead(){

if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  
  //Show UID on serial monitor
  Serial.print("UID tag :");
   content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();

  
  //return ;
  
}



void LiDarSpeed(int speed){

    stepper1.setSpeed(speed);
 if (stepper1.distanceToGo() == 0) 
    stepper1.moveTo(-stepper1.currentPosition());
 stepper1.runSpeed();
  
}


void PulsePump(int time){



digitalWrite(3,HIGH);
delay(time*1000);
//digitalWrite(3,LOW);
//delay(2000);


}


void receiveEvent(){

LED=Wire.read();
Serial.println("Pump yes");

}




void setup()
{ 

pinMode(A0,INPUT);


Wire.begin(12);
 //Wire.onRequest(requestEvent); 
  
  // Function to run when data received from master
  Wire.onReceive(receiveEvent);






 Serial.begin(9600);   // Initiate a serial communication
  while (!Serial)
  {}
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate   



  Vacuum.attach(7);
  Vacuum.writeMicroseconds(1000);
//delay(2000);



 stepper1.setMaxSpeed(100000.0);
  stepper1.setAcceleration(1000.0);
  stepper1.setSpeed(10000);
  stepper1.moveTo(100000);  



pinMode(A0,INPUT);


pinMode(7,OUTPUT);

pinMode(3,OUTPUT);

}

void loop()
{


LiDarSpeed(1000);


if(digitalRead(A0)){

SetVacuum(10);

}else{

  SetVacuum(0);
}

if(digitalRead(A1)){

  
}


//digitalWrite(7,LED);

//digitalWrite(3,HIGH);
//timer=millis();
/*
if(millis()-Timer>=(1000))
{
  digitalWrite(3,HIGH);
 Timer =millis();
delay(1000);
Serial.println("No");
}
else{
digitalWrite(3,LOW);
Serial.println("Pump yes");
}

*/

//digitalWrite(3,LOW);

//PulsePump(5);
//delay(1000);

//SetVacuum(15);
//RFIDRead();
//Serial.println("Pump yes");

//delay(5000);
}


