#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include "DHT.h"
#include <Servo.h>
#include <SimpleDHT.h>
#include <MsTimer2.h>
#define DHTPIN 2
#define DHTTYPE DHT11
#define IR_KEY_1 0xFF30CF
#define SERVO_PIN 9
int RECV_PIN = A0;
IRrecv irrecv(RECV_PIN);
decode_results results;
//DHT dht(DHTPIN, DHTTYPE);
SimpleDHT11 dht11;
Servo myservo;


boolean sensor_read=0;
char msg[15];
int state;
int state_flag=0;
int err=SimpleDHTErrSuccess;
byte temp, humi;
//void decodeIR();
void test();


void setup() {
Serial.begin(9600);
irrecv.enableIRIn();
//MsTimer2::set(1000, test);
//MsTimer2::start();
pinMode(10, OUTPUT);
myservo.attach(SERVO_PIN); 
}

void loop() {
 if(irrecv.decode(&results))
        {
//        Serial.println(results.value, HEX);
          if(results.value==0xFF30CF&&state_flag==0)
          { 
           
            digitalWrite(10, HIGH);  
            //Serial.println(cnt);
            //Serial.write(msg);
            myservo.write(180); 
            delay(100);
            myservo.write(0);
             state=1;
             state_flag=1;
            
          if((err=dht11.read(DHTPIN, &humi, &temp, NULL)) !=SimpleDHTErrSuccess){
          sprintf(msg,"%d:%d:%dL", (int)humi, (int)temp, state);
           Serial.write(msg);
          }
           }
           irrecv.resume();
           
          }
          if(results.value==0xFF18E7&&state_flag==1)
          {
            digitalWrite(10, LOW);
            myservo.write(180); 
            delay(100);
            myservo.write(0);
            state=0;
            
          if((err=dht11.read(DHTPIN, &humi, &temp, NULL)) !=SimpleDHTErrSuccess){
          sprintf(msg,"%d:%d:%dL", (int)humi, (int)temp, state);
          Serial.write(msg);
          }
           irrecv.resume();
           state_flag=0;
        } 
       
/*  if(sensor_read){
    byte temp, humi;
    int err=SimpleDHTErrSuccess;
     
    if((err=dht11.read(DHTPIN, &humi, &temp, NULL)) !=SimpleDHTErrSuccess){
      
      sprintf(msg,"%d:%d:%dL", (int)humi, (int)temp,(int)cnt);
      Serial.write(msg);
    }
    sensor_read=0;  
 }
*/

}
void test(){
  sensor_read = !sensor_read;
}

/*void decodeIR(){
  switch(results.value){
    case IR_KEY_1:
       Serial.println(results.value, HEX);
      cnt++;
      
      break;

  defeat:
      break;
    }
}
*/
