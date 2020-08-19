/*
 WiFiEsp test: ClientTest
http://www.kccistc.net/
작성일 : 2019.12.17 
작성자 : IoT 임베디드 KSH
*/
#define DEBUG
//#define DEBUG_WIFI
#define AP_SSID "embsystem"
#define AP_PASS "embsystem0"
#define SERVER_NAME "192.168.1.40"
#define SERVER_PORT 5000  
#define LOGID "KSH_ARD"
#define PASSWD "PASSWD"

#define CDS_PIN A0
#define WIFITX 7  //7:TX -->ESP8266 RX
#define WIFIRX 6 //6:RX-->ESP8266 TX
#define SERVO_PIN 9
#define DHTPIN 11
#define LED_TEST_PIN 12
#define LED_BUILTIN_PIN 13

#define DHTTYPE DHT11   // DHT11,DHT21, DHT22

#define CMD_SIZE 50
#define ARR_CNT 5           

#include "DHT.h"
#include "WiFiEsp.h"
#include "SoftwareSerial.h"
//#include <TimerOne.h>
#include <MsTimer2.h>
#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

typedef struct {
  int year;
  int month;
  int day;
  int hour;
  int min;
  int sec;
} DATETIME;
DATETIME dateTime={0};
char sendBuf[CMD_SIZE];

bool timerIsrFlag = false;
bool updatTimeFlag = false;

unsigned int secCount;
char getSensorId[10];
int sensorTime;
int cdsValue;
DHT dht(DHTPIN, DHTTYPE);
float temp=0.0;
float humi=0.0;
SoftwareSerial wifiSerial(WIFIRX, WIFITX); 
WiFiEspClient client;
Servo myservo;
bool myservoFlag=false;
char lcdLine1[17]="Smart IoT By KSH";
char lcdLine2[17]="WiFi Connecting!";
void setup() {
  // put your setup code here, to run once:
    myservo.attach(SERVO_PIN);  
    myservo.write(0);
    lcd.init();
    lcd.backlight();
    lcdDisplay(0,0,lcdLine1);
    lcdDisplay(0,1,lcdLine2);
      
    pinMode(LED_TEST_PIN, OUTPUT);    //D12
    pinMode(LED_BUILTIN_PIN, OUTPUT); //D13
    Serial.begin(115200); //DEBUG
    dht.begin();
    
    myservo.detach();
    wifi_Setup();

//    Timer1.initialize(1000000);
//    Timer1.attachInterrupt(timerIsr); // timerIsr to run every 1 seconds
    MsTimer2::set(1000, timerIsr); // 1000ms period
    MsTimer2::start();  
}

void loop() {
  // put your main code here, to run repeatedly: 
  if(client.available()) { 
    socketEvent();
  }
  if(timerIsrFlag)
  {

    timerIsrFlag = false; 
    if(!(secCount%5))
    {
      if (!client.connected()) { 
        lcdDisplay(0,1,"Server Down");
        server_Connect();
      }
    } 
    cdsValue = map(analogRead(CDS_PIN),0,1023,0,100); 
    if(!(secCount%5))
    {
      humi = dht.readHumidity(); 
      temp = dht.readTemperature(); 
      sprintf(lcdLine2,"C:%02d T:%02d H:%02d",cdsValue,(int)temp,(int)humi);
      lcdDisplay(0,1,lcdLine2);
#ifdef DEBUG
      Serial.print("Cds: ");
      Serial.print(cdsValue);     
      Serial.print(" Humidity: ");
      Serial.print(humi);
      Serial.print(" Temperature: ");
      Serial.println(temp);
#endif      
    }

    if(sensorTime!= 0 && !(secCount % sensorTime ))  
    {
      sprintf(sendBuf,"[%s]SENSOR@%d@%d@%d\r\n",getSensorId,cdsValue,(int)temp,(int)humi);
/*      char tempStr[5];
      char humiStr[5];
      dtostrf(humi, 4, 1, humiStr);  //50.0 4:전체자리수,1:소수이하 자리수
      dtostrf(temp, 4, 1, tempStr);  //25.1
      sprintf(sendBuf,"[%s]SENSOR@%d@%s@%s\r\n",getSensorId,cdsValue,tempStr,humiStr);
*/    client.write(sendBuf,strlen(sendBuf));
      client.flush();
    }  
    sprintf(lcdLine1,"%02d.%02d  %02d:%02d:%02d",dateTime.month,dateTime.day,dateTime.hour,dateTime.min,dateTime.sec );
    lcdDisplay(0,0,lcdLine1);
    if(updatTimeFlag)
    {
      client.print("[GETTIME]\n");
      updatTimeFlag = false;
    }
    if(myservoFlag)
    {
      myservo.detach();
      myservoFlag = false;  
    }
  }
}
void socketEvent()
{
  int i=0;
  char * pToken;
  char * pArray[ARR_CNT]={0};
  char recvBuf[CMD_SIZE]={0}; 
  int len;
 
  len =client.readBytesUntil('\n',recvBuf,CMD_SIZE); 
  client.flush();
//  recvBuf[len-1] = '\0';
  
#ifdef DEBUG
  Serial.print("recv : ");
//  Serial.write(recvBuf,len);
  Serial.println(recvBuf);
#endif
  pToken = strtok(recvBuf,"[@]");
  while(pToken != NULL)
  {
    pArray[i] =  pToken;
    if(++i >= ARR_CNT)
      break;
    pToken = strtok(NULL,"[@]");
  }
  if(!strncmp(pArray[1]," New",4))  // New Connected
  {
    updatTimeFlag = true;
    return ;
  }
  else if(!strncmp(pArray[1]," Alr",4)) //Already logged
  {
    client.stop();
    server_Connect();
    return ;
  }   
  if(!strcmp(pArray[1],"LED")) {
    if(!strcmp(pArray[2],"ON")) {
      digitalWrite(LED_BUILTIN_PIN,HIGH);
    }
    else if(!strcmp(pArray[2],"OFF")) {
      digitalWrite(LED_BUILTIN_PIN,LOW);
    }
    sprintf(sendBuf,"[%s]%s@%s\n",pArray[0],pArray[1],pArray[2]);
  } else if(!strcmp(pArray[1],"LAMP")) {
    if(!strcmp(pArray[2],"ON")) {
      digitalWrite(LED_TEST_PIN,HIGH);
    }
    else if(!strcmp(pArray[2],"OFF"))
    {
      digitalWrite(LED_TEST_PIN,LOW);
    }
    sprintf(sendBuf,"[%s]%s@%s\n",pArray[0],pArray[1],pArray[2]);
  }else if(!strcmp(pArray[1],"GETSTATE")) {
    if(!strcmp(pArray[2],"DEV")) {
      sprintf(sendBuf,"[%s]DEV@%s@%s\n",pArray[0],digitalRead(LED_BUILTIN_PIN)?"ON":"OFF",digitalRead(LED_TEST_PIN)?"ON":"OFF");
    }
  }
  else if(!strncmp(pArray[1],"GETSENSOR",9)) {
    if(pArray[2] != NULL) {
      sensorTime = atoi(pArray[2]);
      strcpy(getSensorId,pArray[0]);
      return;
    } else {
      sensorTime = 0;
      sprintf(sendBuf,"[%s]%s@%d@%d@%d\n",pArray[0],pArray[1],cdsValue,(int)temp,(int)humi);
    }
  } else if(!strcmp(pArray[0],"GETTIME")) {  //GETTIME
    dateTime.year = (pArray[1][0]-0x30) * 10 + pArray[1][1]-0x30 ;
    dateTime.month =  (pArray[1][3]-0x30) * 10 + pArray[1][4]-0x30 ;
    dateTime.day =  (pArray[1][6]-0x30) * 10 + pArray[1][7]-0x30 ;
    dateTime.hour = (pArray[1][9]-0x30) * 10 + pArray[1][10]-0x30 ;
    dateTime.min =  (pArray[1][12]-0x30) * 10 + pArray[1][13]-0x30 ;
    dateTime.sec =  (pArray[1][15]-0x30) * 10 + pArray[1][16]-0x30 ;
#ifdef DEBUG
    sprintf(sendBuf,"Time %02d.%02d.%02d %02d:%02d:%02d\n\r",dateTime.year,dateTime.month,dateTime.day,dateTime.hour,dateTime.min,dateTime.sec );
    Serial.println(sendBuf);
#endif
    return;
  }  
  else if(!strcmp(pArray[1],"SERVO")) 
  {
    myservoFlag = true;
    myservo.attach(SERVO_PIN);  
    if(!strcmp(pArray[2],"ON"))
      myservo.write(180); 
    else 
      myservo.write(0);
    sprintf(sendBuf,"[%s]%s@%s\n",pArray[0],pArray[1],pArray[2]);
  } 
  client.write(sendBuf,strlen(sendBuf));
  client.flush();


#ifdef DEBUG
  Serial.print(", send : ");
  Serial.print(sendBuf);
#endif
}
void timerIsr()
{
//  digitalWrite(LED_BUILTIN_PIN,!digitalRead(LED_BUILTIN_PIN));
  timerIsrFlag = true;
  secCount++;
  clock_calc(&dateTime);
}
void clock_calc(DATETIME *dateTime)
{
  int ret = 0;
  dateTime->sec++;          // increment second

  if(dateTime->sec >= 60)                              // if second = 60, second = 0
  { 
      dateTime->sec = 0;
      dateTime->min++; 
             
      if(dateTime->min >= 60)                          // if minute = 60, minute = 0
      { 
          dateTime->min = 0;
          dateTime->hour++;                               // increment hour
          if(dateTime->hour == 24) 
          {
            dateTime->hour = 0;
            updatTimeFlag = true;
          }
       }
    }
}
void wifi_Setup() {
  wifiSerial.begin(9600);
  wifi_Init();
  server_Connect();
}
void wifi_Init()
{
  do {
    WiFi.init(&wifiSerial);
    if (WiFi.status() == WL_NO_SHIELD) {
#ifdef DEBUG_WIFI    
      Serial.println("WiFi shield not present");
#endif 
    }
    else
      break;   
  }while(1);

#ifdef DEBUG_WIFI    
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(AP_SSID);
#endif     
  while(WiFi.begin(AP_SSID, AP_PASS) != WL_CONNECTED) {   
#ifdef DEBUG_WIFI  
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(AP_SSID);   
#endif   
  }
  sprintf(lcdLine1,"ID:%s",LOGID);  
  lcdDisplay(0,0,lcdLine1);
  sprintf(lcdLine2,"IP:%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
  lcdDisplay(0,1,lcdLine2);
  
#ifdef DEBUG_WIFI      
  Serial.println("You're connected to the network");    
  printWifiStatus();
#endif 
}
int server_Connect()
{
#ifdef DEBUG_WIFI     
  Serial.println("Starting connection to server...");
#endif  

  if (client.connect(SERVER_NAME, SERVER_PORT)) {
#ifdef DEBUG_WIFI     
    Serial.println("Connected to server");
#endif  
    client.print("["LOGID":"PASSWD"]"); 
  }
  else
  {
#ifdef DEBUG_WIFI      
     Serial.println("server connection failure");
#endif    
  } 
}
void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
void lcdDisplay(int x, int y, char * str)
{
  int len = 16 - strlen(str);
  lcd.setCursor(x,y);
  lcd.print(str);
  for(int i=len;i>0;i--)
    lcd.write(' ');
}
