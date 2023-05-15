#include <dummy.h>

//#include <MPU6050.h>
#include <MPU6050_light.h>
#include <Wire.h>

#include "esp_err.h"
#include "esp_attr.h"
#include "soc/soc.h"
#include "soc/timer_periph.h"
#include "esp_intr_alloc.h"
#include "hal/timer_types.h"

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

const char* host = "AGV";
const char* ssid = "GalaxyS204c90";
const char* password = "lwkm7349";


WebServer server(80);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



const char* loginIndex =
 "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
        "<tr>"
            "<td colspan=2>"
                "<center><font size=4><b>ESP32 Login Page</b></font></center>"
                "<br>"
            "</td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
             "<td>Username:</td>"
             "<td><input type='text' size=25 name='userid'><br></td>"
        "</tr>"
        "<br>"
        "<br>"
        "<tr>"
            "<td>Password:</td>"
            "<td><input type='Password' size=25 name='pwd'><br></td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
            "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
        "</tr>"
    "</table>"
"</form>"
"<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='admin' && form.pwd.value=='admin')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    " alert('Error Password or Username')/*displays error message*/"
    "}"
    "}"
"</script>";

/*
 * Server Index Page
 */

const char* serverIndex =
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')"
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






bool START =true;



portMUX_TYPE synch1 = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE synch2 = portMUX_INITIALIZER_UNLOCKED;

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
MPU6050 mpu(Wire);
int Kp=1.2;
unsigned int reference =0;

const float R=0.0508;            //// R is the wheel radius in meter
const int reduction_ratio=9;
const int Steps_per_rotation= 200*reduction_ratio;
const float Meter_per_rotation=0.36424;                             /////2*3.14*R;
const float Steps_per_meter =5642.2;                             ////(Steps_per_rotation)/(Meter_per_rotation);


const float full_rotation_path = 1.036;
const  float full_rotation_steps = full_rotation_path*Steps_per_meter;


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





void WIFISet(){


 // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();


}






void angleCorrect(int angle){
    mpu.update();
    //angle=angle+reference;
    int error = angle-mpu.getAngleZ();
    double Kp2=1000;
     int correct= Kp2*error;
 
 
  int perverror=0;
  while(abs(error)>=1){
      mpu.update();
      error = angle-mpu.getAngleZ();
      if(error>0){
         digitalWrite(m1dir,LOW);
         digitalWrite(m2dir,LOW);
      } else if(error<0){
         digitalWrite(m1dir,HIGH);
         digitalWrite(m2dir,HIGH);

      }


      digitalWrite(LED, !digitalRead(LED));
      digitalWrite(LED2, !digitalRead(LED2));
      delayMicroseconds(500);
     
      Serial.println(error);
 
          }

      perverror=error;
      delay(5);
      while(abs(error)>0){
      mpu.update();
      error = angle-mpu.getAngleZ();
      if(error>0){
         digitalWrite(m1dir,LOW);
         digitalWrite(m2dir,LOW);
      } else if(error<0){
         digitalWrite(m1dir,HIGH);
         digitalWrite(m2dir,HIGH);

      }


      digitalWrite(LED, !digitalRead(LED));
      digitalWrite(LED2, !digitalRead(LED2));
      delayMicroseconds(2000);
     
      Serial.println(error);
 
          }




     }
  //while(1){delay(10);}
    //delay(15000);

 // }


//*/




void Rotate(float angle){

     M1taskcomplete =false;
     M2taskcomplete =false;
     int steps = abs(1.6*((angle*full_rotation_steps)/360));
     angle=angle+reference;
  if(angle>reference){
     M1MoveRPM(20,steps,false);
     M2MoveRPM(20,steps,true);  
     
  } else if(angle<reference){
     M1MoveRPM(20,steps,true);
     M2MoveRPM(20,steps,false); 
      
  }
   
   while(!M1taskcomplete  || !M2taskcomplete){
   mpu.update();
   delay(5);
   Serial.println(full_rotation_steps);

   }
   Serial.println(angle);
   Serial.println(steps);
   delay(300);

angleCorrect(angle);
reference=angle;

}











void IRAM_ATTR onTimer1(){

  portENTER_CRITICAL(&synch1);

digitalWrite(LED, !digitalRead(LED));

i1++;

if(i1>=motorsteps1)
  {    i1=0;
       M1taskcomplete = true;
       timerEnd(Motor1);
       //timerDetachInterrupt(Motor1);
       //timerAlarmDisable(Motor1);
     // if(M2taskcomplete==true){
       //angleCorrect(angle);}


    
       
      
 }

portEXIT_CRITICAL(&synch1);

}


void IRAM_ATTR onTimer2(){

portENTER_CRITICAL(&synch2);

digitalWrite(LED2, !digitalRead(LED2));

i2++;

if(i2>=motorsteps2)
  {
     i2=0;
     M2taskcomplete =true;
     timerEnd(Motor2);
          // timerDetachInterrupt(Motor2);
          // timerAlarmDisable(Motor2);
      // if(M1taskcomplete==true){
       //  }

     //angleCorrect(angle);
      //mpu.getAngleZ();
      //mpu.update();
   
  }

portEXIT_CRITICAL(&synch2);

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


void M1MoveRPM(volatile float rpm,int steps,bool dir){

if(dir)
  digitalWrite(m1dir,HIGH);
else
 digitalWrite(m1dir,LOW);



motorsteps1=2*steps;
volatile int t = 50000/(3*rpm);
settimer1(t);



}
void M2MoveRPM(volatile float rpm,int steps,bool dir){

if(dir)
  digitalWrite(m2dir,LOW);
else
 digitalWrite(m2dir,HIGH);


motorsteps2=2*steps;
volatile int t = 50000/(3*rpm);
settimer2(t);



}











void MoveForward(double distance,bool direction, double speed){
mpu.update();
M1taskcomplete =false;
M2taskcomplete =false;
int reference_angle =mpu.getAngleZ();

int steps=distance*Steps_per_meter;
int rpm = (speed*60)/0.36424 ; 
//double t = (a/360)*(60/2*rpm)*(1/reduction_ratio)*1000000;      ////time in microseconds
  
M1MoveRPM(rpm,steps,true);
M2MoveRPM(rpm,steps,true);
int perverror=0;

while(!M1taskcomplete  || !M2taskcomplete ){
   
   mpu.update();
   int error= reference_angle-mpu.getAngleZ();
   
   if(error!=perverror){

   if(abs(error)>=1){
       double correction=0.7*error;
       M1MoveRPM(rpm-correction,steps,direction);
       M2MoveRPM(rpm+correction,steps,direction);

   }}
   
    perverror=error;
    delay(5);
}
       timerEnd(Motor1);
       timerEnd(Motor2);
     
     angleCorrect(reference);
//delay(5000);

}



void END(){
  
  START=false;
  
while(!START){
  server.handleClient();
  delay(10);
}


}





void setup() {


//timer_init();

 Wire.begin();

 WIFISet();


   /////////---------------------GyroScope------------------------------///////////

    byte status = mpu.begin();
    while(status!=0){ }
    delay(1000);
  mpu.calcOffsets(true,true); 
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

//MoveForward(1,true,1);

}

void loop() {
 server.handleClient();
  delay(1);




//M1taskcomplete =false;
//M2taskcomplete =false;


MoveForward(1,true,0.1);

END();

/*
Rotate(90-1.27);

//87.8

delay(1000);
MoveForward(0.5,true,0.2);
Rotate(90-1.27);
delay(1000);
MoveForward(1,true,0.2);
Rotate(-90+1.27);
delay(1000);
MoveForward(0.5,true,0.2);
Rotate(-90+1.27);

*/


//mpu.update();
//Serial.println(mpu.getAngleZ());


//MoveForward(1,true,1);

/*

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

*/


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
