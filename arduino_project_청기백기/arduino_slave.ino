#include <LiquidCrystal_I2C.h>

#include <SoftwareSerial.h>
#include <Servo.h>
#include <MsTimer2.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo servo_1;
Servo servo_2;



char str[100];
char *sArr[5] = {NULL, };
SoftwareSerial T_Serial(3, 4);



volatile byte state = HIGH;
int duration = 500;
unsigned long pre_time = 0;
unsigned long cur_time = 0;



void isr()
{

  cur_time = millis();
  if (cur_time - pre_time >= duration) {
    state = !state;
    pre_time = cur_time;
  }

}


void setup() {
  T_Serial.begin(9600);
  Serial.begin(9600);
  servo_1.attach(9);
  servo_2.attach(10);


  pinMode(13, OUTPUT);


  attachInterrupt(0, isr, LOW);

  lcd.init();
  lcd.backlight();

}







void loop() {
  int index = 0, i = 0;
  if (state) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("MOVING TIME");
    
  while (T_Serial.available())
  {
    str[index++] = T_Serial.read();
    str[index] = NULL;
  }

  char* ptr = strtok(str, ",");
  
  while (ptr != NULL)
  {
    sArr[i] = ptr;
    i++;
    ptr = strtok(NULL, " ");
  }

  Serial.print(sArr[0]);
  Serial.print("   ");
  Serial.println(sArr[1]);


  int a = atoi(sArr[0]);
  int b = atoi(sArr[1]);

  servo_1.write(a);
  servo_2.write(b);

  delay(100);
/*
  if (state) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("MOVING TIME");
  }
  */
  } else if (!state) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("REST TIME");
  }

}
