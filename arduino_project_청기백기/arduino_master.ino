#include <SoftwareSerial.h>

int TX=3;
int RX=4;

String str;
  
SoftwareSerial T_Serial(TX,RX);

void setup() {
  
Serial.begin(9600);
T_Serial.begin(9600);

}

void loop() { 
  
  int pin1=A0;
  int pin2=A1;

  
  int val=analogRead(pin1);
  val=map(val,0,1023,0,179);

  int val2=analogRead(pin2);
  val2=map(val2,0,1023,0,179);

  String mystring=String(val);
  String mystring2=String(val2);

  str=mystring+","+mystring2;


  Serial.println(str);
  T_Serial.print(str);
  delay(700);

}
