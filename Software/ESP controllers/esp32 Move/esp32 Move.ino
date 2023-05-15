//#include <MPU6050.h>

#include <Wire.h>



//#include <MPU6050_light.h>



int i1 = 0;
int i2 =0;
int motorsteps1=0;
int motorsteps2=0;
#define m1dir 5
#define m2dir 18
#define LED 4
#define LED2 15
bool M1taskcomplete = false;
bool M2taskcomplete =false;
hw_timer_t *Motor1 = NULL;
hw_timer_t *Motor2 = NULL;
hw_timer_t *MPUU =NULL;
int angle=0;
//MPU6050 mpu(Wire);
int Kp=100;


/*

double R=0.0508;            //// R is the wheel radius in meter
int reduction_ratio=9;
int Steps_per_rotation= 200*reduction_ratio;
int Meter_per_rotation=2*3.14*R;
int Steps_per_meter=Steps_per_rotation/Meter_per_rotation;
*/

/*

double t = (a/360)*(60/2*rpm)*(1/reduction_ratio)*1000000;      ////time in microseconds
rpm = (speed*60)/Meter_per_rotation ;                           ////Speed in m/s


*/
//
///*
void angleCorrect(int angle){

 // int error = angle-mpu.getAngleZ();
  



  //while(1){
     // mpu.update();
      //error = angle-mpu.getAngleZ();
  //    int correct= Kp*error;
      //Serial.println(error);
      //M2MoveRPM(15,correct,true);
      //M1MoveRPM(15,correct,false);
      
      //while(!M1taskcomplete){delay(5);
            // timerEnd(Motor1);
            // timerEnd (Motor2);

      
    // }
  //while(1){delay(10);}
    //delay(15000);

  }


//*/


void IRAM_ATTR onTimer1(){

digitalWrite(LED, !digitalRead(LED));

i1++;

if(i1>=motorsteps1)
  {    i1=0;
       M1taskcomplete = true;
       timerEnd(Motor1);
      if(M2taskcomplete==true){
       angleCorrect(angle);}


    
       
      
  }

}


void IRAM_ATTR onTimer2(){

digitalWrite(LED2, !digitalRead(LED2));

i2++;

if(i2>=motorsteps2)
  {
     i2=0;
     M2taskcomplete =true;
     timerEnd(Motor2);
       if(M1taskcomplete==true){
         angleCorrect(angle);}


    
   
  }

}



void settimer1(int timeD){

Motor1 = timerBegin(0, 80, true);
timerAttachInterrupt(Motor1, &onTimer1, true);
timerAlarmWrite(Motor1, timeD, true);
timerAlarmEnable(Motor1); //Just Enable

}

void settimer2(int timeD){

Motor2 = timerBegin(1, 80, true);
timerAttachInterrupt(Motor2, &onTimer2, true);
timerAlarmWrite(Motor2, timeD, true);
timerAlarmEnable(Motor2); //Just Enable

}


void M1MoveRPM(float rpm,int steps,bool dir){

if(dir)
  digitalWrite(m1dir,HIGH);
else
 digitalWrite(m1dir,LOW);



motorsteps1=2*steps;
int t = 50000/(3*rpm);
settimer1(t);



}
void M2MoveRPM(float rpm,int steps,bool dir){

if(dir)
  digitalWrite(m1dir,HIGH);
else
 digitalWrite(m1dir,LOW);


motorsteps2=2*steps;
int t = 50000/(3*rpm);
settimer2(t);



}

void IRAM_ATTR MPU(){
 // mpu.update();
}

void setup() {


 Wire.begin();




   /////////---------------------GyroScope------------------------------///////////

    //byte status = mpu.begin();
   // while(status!=0){ }
   // delay(1000);
    //mpu.calcOffsets(true,true); 
  /////////---------------------------------------------------///////////


//MPUU = timerBegin(3, 80, true);
//timerAttachInterrupt(MPUU, &MPU, true);
//timerAlarmWrite(MPUU, 100000000, true);
//timerAlarmEnable(MPUU);





Serial.begin(115200);
pinMode(LED,OUTPUT);
pinMode(LED2,OUTPUT);
pinMode(m1dir,OUTPUT);
pinMode(m2dir,OUTPUT);



}

void loop() {
M1taskcomplete =false;
M2taskcomplete =false;




M2MoveRPM(40,7000,true);
M1MoveRPM(40,7000,true);
while(!M1taskcomplete){delay(5);}

delay(1000);
M1taskcomplete =false;
M2taskcomplete =false;
M2MoveRPM(25,4500/2,true);
M1MoveRPM(25,4500/2,false);
while(!M2taskcomplete){delay(5);}
delay(1000);  




//*/
/* M1taskcomplete =false;
M2taskcomplete =false;

M2MoveRPM(30,2000,true);
M1MoveRPM(30,2000,true);
while(!M1taskcomplete){delay(1);}

delay(100);
M1taskcomplete =false;
M2taskcomplete =false;
M2MoveRPM(25,400,false);
M1MoveRPM(25,400,true);
while(!M1taskcomplete){delay(1);}
delay(100);
 M1taskcomplete =false;
M2taskcomplete =false;

M2MoveRPM(40,20000,true);
M1MoveRPM(40,20000,true);
while(!M1taskcomplete){delay(1);}

delay(100);
M1taskcomplete =false;
M2taskcomplete =false;
M2MoveRPM(25,2000,true);
M1MoveRPM(25,2000,false);
while(!M1taskcomplete){delay(1);}
delay(100);
M1taskcomplete =false;
M2taskcomplete =false;

M2MoveRPM(30,500,true);
M1MoveRPM(30,500,true);
while(!M1taskcomplete){delay(1);}

delay(100);
M1taskcomplete =false;
M2taskcomplete =false;
M2MoveRPM(25,400,true);
M1MoveRPM(25,400,false);
while(!M1taskcomplete){delay(1);}
delay(100);
*/
}



