#include <Arduino.h>

#include <PIDController.h>
#include <Wire.h> 
#include <MPU6050_light.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

////#define SCREEN_WIDTH 128 // OLED display width, in pixels
////#define SCREEN_HEIGHT 64 // OLED display height, in pixels
////Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
// RFID Pins



#define SS_PIN 53
#define RST_PIN 46 //reset pin.

//Encoders interrupts
#define encoderA 2
#define encoderB 3

//---------------------------------------------------Encoders A and B counters-------------------------------
volatile unsigned int encoderA_pos = 0; 
volatile unsigned int encoderB_pos = 0;

//---------------------------------------------------Gyro PID------------------------------------------------
volatile unsigned int mspeed = 150; // The needed speed of motors while moving forward PWM signal (out of 255).

volatile  int powerA = 0 ; // the speed needed to correct orientation.
volatile  int powerB = 0 ;

volatile double Kp = 1.2367 ;   // the constant tunings for the PID
volatile double Ki = 1.5736 ; 
volatile double Kd = 0.058  ;


volatile double integral = 0  ;   // initialize variables of PID
volatile double derivative = 0 ;
volatile double error = 0 ;
volatile double lastError = 0 ;
volatile double correction= 0;
//-------------------------------------------------Gyro-------------------------------------------------------
MPU6050 mpu(Wire);
unsigned long timer = 0; // create timer to keep getting the angle from gyro every ms.
volatile int count = 0 ; 
volatile int rotate= 0 ;
// Motor A 
const unsigned int EN_A = 5;
const unsigned int IN1_A = 6;
const unsigned int IN2_A = 7;
 
// Motor B 
const unsigned int EN_B = 10 ;
const unsigned int IN1_B = 9 ;
const unsigned int IN2_B = 8 ;
//-------------------------------------------------------Pump-------------------------------------------------
//const unsigned int pwm_pump = 1 0 ;
const unsigned int pump1 = 38 ;
const unsigned int pump2 = 39 ;
//-------------------------------------------------------Suction motors-------------------------------------------------
const unsigned int pwm_smotor1 =  11;
const unsigned int smotor1 = 44 ;
const unsigned int smotor2 = 45 ;

const unsigned int pwm_smotor2 =  12;
const unsigned int smotor3 = 42 ;
const unsigned int smotor4 = 43 ;
//-------------------------------------------------------Brushes----------------------------------------------
//const unsigned int pwm_brush1 =  11;
const unsigned int bmotor1 = 40 ;
const unsigned int bmotor2 = 41 ;

//-------------------------------------------------Create MFRC522 instance.RFID------------------------------- 
MFRC522 mfrc522(SS_PIN, RST_PIN);  
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------  
void setup() {
  Serial.begin(9600);
//    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))  // Address 0x3C for most of these displays, if doesn't work try 0x3D 
//  { 
//    Serial.println(F("SSD1306 allocation failed"));
//    for(;;);                                      // Don't proceed, loop forever
//  } 
//  Wire.begin();
//  display.setTextSize(1);             
//  display.setTextColor(SSD1306_WHITE);            // Draw white text
//  display.clearDisplay();  
//  
//  display.println(F("Calculating gyro offset, do not move MPU6050"));
//  display.display();       
//  display.setTextSize(2);                       
  //------------------initialize digital pins-----------------------------------------------------------------
  pinMode(IN1_A, OUTPUT);
  pinMode(IN2_A, OUTPUT);
  pinMode(IN1_B, OUTPUT);
  pinMode(IN2_B, OUTPUT);
  pinMode(EN_A, OUTPUT);
  pinMode(EN_B, OUTPUT);
//  pinMode(smotor1,OUTPUT);
//  pinMode(smotor2,OUTPUT);
//  pinMode(pwm_smotor,OUTPUT);
  pinMode(encoderA, INPUT);
  pinMode(encoderB, INPUT);
  
  //--------------------ISR on rising edge. ENCODER-----------------------------------------------------------
  attachInterrupt(digitalPinToInterrupt(encoderA), HallEffect_encoder, RISING);
  attachInterrupt(digitalPinToInterrupt(encoderB), HallEffect_encoder, RISING);
  
  //------------------Gyro Mpu6050----------------------------------------------------------------------------
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(); // gyro and accelero
  Serial.println("Done!\n");
  
  //------------------RFID------------------------------------------------------------------------------------
  Serial.begin(9600);   // Initiate a serial communication
  while (!Serial)
  {}
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate   
}

//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------

void Start_pump(){
  
  if(rotate==0 ){
    
  digitalWrite(pump1, LOW);
  digitalWrite(pump2, HIGH);
    }
   if (rotate ==1 )
   {
     digitalWrite(pump1, LOW);
     digitalWrite(pump2, LOW); 
    }
  }
//------------------------------------------------------------------------------------------------------------
void Start_bmotor(){
  
  digitalWrite(bmotor1, HIGH);
  digitalWrite(bmotor2, LOW);
  
  }
//------------------------------------------------------------------------------------------------------------
void Start_smotor(){
    digitalWrite(smotor1, HIGH);
    digitalWrite(smotor2, LOW);
    analogWrite(pwm_smotor1,255);
  
    digitalWrite(smotor3, HIGH);
    digitalWrite(smotor4, LOW);
    analogWrite(pwm_smotor2,255);
  }
//------------------------------------------------------------------------------------------------------------
void HallEffect_encoder(){

  if(digitalRead(encoderA) == HIGH){
    encoderA_pos++;
  }else{
    //encoderA_pos--;
  }
  if(digitalRead(encoderB) == HIGH){
    encoderB_pos++;
  }else{
    //encoderB_pos--;
  }
}
//------------------------------------------------------------------------------------------------------------

void RFID_Start(){
//  
//    display.clearDisplay();
//    display.setCursor(0,0);
//    display.println("RFID ENTER");
//    display.display();
  // Look for new cards
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
  String content= "";
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

  
   if (content.substring(1) == "39 4C 05 E5" || content.substring(1) == "0C 71 A8 E9" || content.substring(1) == "D4 16 A8 E9" || content.substring(1) == "43 8A 8C 3E" || content.substring(1) == "76 74 0B 29") //***********************<90 DEGREE Left>***********************
  {
    rotate = 1;
    
//    display.clearDisplay();
//    display.setCursor(0,0);
//    display.println("RFID READ11");
//    display.display();
    MotorStop();
    delay(300);
    Rotate90R(480,150); //rotate 90 to the right with halleffect
    delay(300);
    rotate = 0;
          
   }
     else if (content.substring(1) == "53 17 74 3E" || content.substring(1) == "53 4A D5 3E" || content.substring(1) == "53 E3 E3 3E" || content.substring(1) == "33 7A 83 3E") //***********************<90 DEGREE Right>***********************
  {
    rotate = 1;
 
//    display.clearDisplay();
//    display.setCursor(0,0);
//    display.println("RFID READ22");
//    display.display();
    MotorStop();
    delay(300);
    Rotate90L(480,150); //rotate 90 to the right with halleffect
    delay(300);
     
    rotate = 0;    
    
   }
//   else if (content.substring(1) == "86 45 30 29" ) //***********************<END Cycle>***********************
//  {
//    rotate = 1;
//    MotorStop();
//            
//   }
     
  }
//-----------------------------------
void PIDStartGyro(){
 if(rotate == 0){ 
   mpu.update(); // starts and updates the gyro mpu6050 eachtime this method is called 
   error = mpu.getAngleZ() ; // (P)roportional
   timer = millis(); 
   
   if (error == 0){
       integral = 0 ;
   }
   
   else{
    integral = integral + error ;
    derivative = error - lastError  ;
    }
   
  correction = ((Kp*(error)) + (Ki*(integral)) + (Kd*(derivative))) * -1; // the correct value that should be added or reduced(depending on the angle of the gyro) from the main motor speed to correct orientation.

  powerB = (mspeed - 25) - correction ;
  powerA = (mspeed) + correction ; 
  if (error == 0){
    powerA=mspeed;
    powerB=(mspeed - 25);     
    }
  if (error < 2 & error > -2){
    powerA=mspeed ;
    powerB=(mspeed - 25) ; 
   }
  if (powerA > 255){
    powerA = 255;
    MotorForwardA(powerA);
    }
  
  if (powerB > 230){
    powerB = 230;
    MotorForwardB(powerB);
    }  
  if (powerA >0 ){
    MotorForwardA(powerA);
    }
  else {
    powerA= 0;
    MotorForwardA(powerA);
    }
  if (powerB >0){
    MotorForwardB(powerB);
    }
  else{
    powerB= 0 ;
    MotorForwardB(powerB);
    }
    
//  display.clearDisplay();
//  display.setCursor(0,0);  
//     
//  display.print("A : ");
//  display.println(powerA);
//  display.print("B : ");
//  display.println(powerB);
//  display.println(error);
//  display.display();
   lastError = error ; //save last error for (D)erivative to predict future errors.
     
   delay(10);
 }
}
//-------------------------------------------------------------------------------------------------------------------------------
void MotorForwardA(int mspeed){
  
    digitalWrite(IN1_A, HIGH);
    digitalWrite(IN2_A, LOW);
    analogWrite(EN_A, mspeed); 
}

//-------------------------------------------------------------------------------------------------------------------------------
void MotorForwardB(int mspeed){
  
    digitalWrite(IN1_B, HIGH);
    digitalWrite(IN2_B, LOW);
    analogWrite(EN_B, mspeed); 
}

//-------------------------------------------------------------------------------------------------------------------------------
void MotorStop(){
  //Stop motors A and B.
  digitalWrite(IN1_A, LOW);
  digitalWrite(IN2_A, LOW);
  digitalWrite(IN1_B, LOW);
  digitalWrite(IN2_B, LOW);
  analogWrite(EN_A,0);
  analogWrite(EN_B, 0);
}

//-------------------------------------------------------------------------------------------------------------------------------
void MotorBackwardA(int mspeed){
  if(mspeed > 100){
    digitalWrite(IN1_A, LOW);
    digitalWrite(IN2_A, HIGH);
    analogWrite(EN_A, mspeed);
    
  }else{
   digitalWrite(IN1_A, LOW);
   digitalWrite(IN2_A, LOW);
  }
}

//-------------------------------------------------------------------------------------------------------------------------------
void MotorBackwardB(int mspeed){
  if(mspeed > 100){
    digitalWrite(IN1_B, LOW);
    digitalWrite(IN2_B, HIGH);
    analogWrite(EN_B, mspeed);
  
  }else{
   digitalWrite(IN1_B, LOW);
   digitalWrite(IN2_B, LOW);
   
  }
}
//-------------------------------------------------------------------------------------------------------------------------------
  // Functions to rotate90 Left and Right
  void Rotate90L(int steps, int mspeed) 
  {
    encoderA_pos = 0;  //  reset counter A to zero
    encoderB_pos = 0;  //  reset counter B to zero
    
    // Set Motor A to stop
    digitalWrite(IN1_A, LOW);
    digitalWrite(IN2_A, LOW);
 
    // Set Motor B forward
    digitalWrite(IN1_B, HIGH);
    digitalWrite(IN2_B, LOW);

    
   
     // Go until step value is reached
    while (steps > encoderB_pos ) {
      if  (steps > encoderB_pos ) {
      analogWrite(EN_B, mspeed);

      } else {
      analogWrite(EN_B, 0); 
      }
    }
    
    // Stop when done
    analogWrite(EN_A, 0);
    analogWrite(EN_B, 0);
    encoderA_pos = 0;  //  reset counter A to zero
    encoderB_pos = 0;  //  reset counter B to zero
 
  }
//-------------------------------------------------------------------------------------------------------------------------------
  void Rotate90R(int steps, int mspeed) 
  {
    encoderA_pos = 0;  //  reset counter A to zero
    encoderB_pos = 0;  //  reset counter B to zero
    volatile int check = 0 ;
    // Set Motor A forward
    digitalWrite(IN1_A, HIGH);
    digitalWrite(IN2_A, LOW);
 
    // Set Motor B to stop
    digitalWrite(IN1_B, LOW);
    digitalWrite(IN2_B, LOW);

   
    // Go until step value is reached
    while (steps > encoderA_pos ) {
      if (steps > encoderA_pos  ) {
      analogWrite(EN_A, mspeed);
      } else {
      analogWrite(EN_A, 0);
      }
    }
    
    // Stop when done
    analogWrite(EN_A, 0);
    analogWrite(EN_B, 0);
    encoderA_pos = 0;  //  reset counter A to zero
    encoderB_pos = 0;  //  reset counter B to zero
 
  }
//------------------------------------------------------------------------------------------------------------------------------- 
  // Functions to rotate90 Left and Right REVERSE
  void Rotate90RR(int steps, int mspeed) 
  {
    encoderA_pos = 0;  //  reset counter A to zero
    encoderB_pos = 0;  //  reset counter B to zero
   
    // Set Motor A to stop
    digitalWrite(IN1_A, LOW);
    digitalWrite(IN2_A, LOW);
 
    // Set Motor B Reverse
    digitalWrite(IN1_B, LOW);
    digitalWrite(IN2_B, HIGH);
   
   // Go until step value is reached
    while (steps > encoderB_pos) {
      if (steps > encoderB_pos) {
      analogWrite(EN_B, mspeed);
      } else {
      analogWrite(EN_B, 0);
      }
    }
    
    // Stop when done
    analogWrite(EN_A, 0);
    analogWrite(EN_B, 0);
    encoderA_pos = 0;  //  reset counter A to zero
    encoderB_pos = 0;  //  reset counter B to zero
 
  }
//-------------------------------------------------------------------------------------------------------------------------------
  void Rotate90LR(int steps, int mspeed) 
  {
    encoderA_pos = 0;  //  reset counter A to zero
    encoderB_pos = 0;  //  reset counter B to zero
   
    // Set Motor A Reverse
    digitalWrite(IN1_A, LOW);
    digitalWrite(IN2_A, HIGH);
 
    // Set Motor B to stop
    digitalWrite(IN1_B, LOW);
    digitalWrite(IN2_B, LOW);
   
    // Go until step value is reached
    while (steps > encoderA_pos) {
      if (steps > encoderA_pos) {
      analogWrite(EN_A, mspeed);
      } else {
      analogWrite(EN_A, 0);
      }
    }
    
    // Stop when done
    analogWrite(EN_A, 0);
    analogWrite(EN_B, 0);
    encoderA_pos = 0;  //  reset counter A to zero
    encoderB_pos = 0;  //  reset counter B to zero
 
  }
//-------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------
void loop() {
  
  PIDStartGyro();
  RFID_Start(); 
  Start_smotor();
  Start_bmotor();
  Start_pump();
}
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------