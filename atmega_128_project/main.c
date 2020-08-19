/*
 * atmega_128_project.c
 *
 * Created: 2020-05-17 오전 11:15:00
 * Author : admin
 */ 

#define F_CPU 14745600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "wifi.h"
#include "uart.h"
#include "adc.h"
#include "dht11.h"
#include "lcd.h"


//PF0~2 C0~2
//PF4~7 R0~R3


#define MOTOR_DDR DDRB
#define MOTOR_PORT PORTB//PB4(OC0) PWM	: Dc motor M_EN
#define MOTOR_LEFT_ENABLE PB2//  Dc motor M_D1
#define MOTOR_RIGHT_ENABLE PB3 //  Dc motor M_D2
FILE OUTPUT = FDEV_SETUP_STREAM((void *)UART0_transmit, NULL, _FDEV_SETUP_WRITE);

#define ROTATION_DELAY	1000
#define PULSE_MIN 1300
#define PULSE_MID 3000
#define PULSE_MAX 4700

unsigned char key_input();

extern volatile unsigned long systick_count;
extern volatile unsigned long systick_sec;
void TIMER0_init();

#define ARR_CNT 7
extern volatile int rx0flag;
extern volatile int rx1flag;
extern int connect_flag;
extern char uart1_rxdata[5][BUFSIZE];


#define bit_set(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#define bit_clear(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))

#define DIGIT1 1
#define DIGIT2 2
#define DIGIT3 3
#define DIGIT4 4
#define FND_DDR DDRC
#define FND_PORT PORTC
#define FND_DIGIT_DDR DDRG
#define FND_DIGIT_PORT PORTG
void display_fnd(int, int);
void display_digit(int positon,int number);
char fnd[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x27,0x7f,0x6f};
volatile int fndnum;
extern volatile int cdsValue;
void InitiallizeTimer(void)
{
	TCCR1A =_BV(WGM11) | _BV(COM1A1);
	TCCR1B = _BV(WGM12)| _BV(WGM13) | _BV(CS11);
	
	ICR1=39999;
}

//////////////////////////////////////////////////////////////////////////
////// passward 추가  ////
//////////////////////////////////////////////////////////////////////////
#define COL_OUT	PORTF		// 열 출력
#define ROW_IN		PINF		// 행 입력


int read_key(void);
volatile char input_key_vlaue[7]={0};
volatile char key_password[7]={'*','1','1','1','1','#','\0'};
const char key_setting[7]={'*','0','0','0','0','#','\0'};
volatile char input_temp=0;
volatile int k=0;
void key_set_0(void);
void key_pad_change(int value);
void passwrad_change(void);

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////




int main(void)
{
    /* Replace with your application code */
	int i;
	int key;
	int serverflag = 0;
	int sensorTime = 0;
	int servoFlag =0;
	int fcmCdsFlag = 0;
//	int ledflag = 0;
	unsigned long systick_sec_old=0;
	char *pToken;
	char *pArray[ARR_CNT]={0};
	char senddata[100]={0};
	char recvdata[100]={0};
	char getSensorId[20]={0};
	struct dht11 dhtInfo; 
	int duty = 50;
	int old_duty = duty;	//duty:50%
	int ocr0=0;
	
	DDRB |= (1<< PB5);
	
	InitiallizeTimer();
	
	FND_DDR = 0xff;
	FND_DIGIT_DDR = 0x0f;
	DDRA = 0x0f;		//PA0~PA3 : LED
	PORTA = 0xf0;       //PA4~PA7 : KEY(internal pull up)
	MOTOR_DDR = 0xff;
	MOTOR_PORT = 0x00;
	
	
	
	TIMER0_init();		//250Hz timer0
	UART0_init();
	UART1_init();
	//CDS_init();			//ADC0
	//DHT_Init(&dhtInfo, PORTB3);					 // 온습도 센서 초기화 
	Port_Init();
	Init_LCD();
	clrscr();
	
	
	sei(); //global interrupt enable
	stdout=&OUTPUT;
	UART0_string("main start!!\r\n");

	
	WIFI_init();
	ocr0 = (256/100.0) * duty;
	if(ocr0 >= 256)
		ocr0=255;
	OCR0 = ocr0;
	
	bit_clear(MOTOR_PORT,MOTOR_LEFT_ENABLE);
	bit_clear(MOTOR_PORT,MOTOR_RIGHT_ENABLE);
	
	clrscr();
	cli();//
	lcd(0,0,"Hello");
	sei();
	
	
	
	
	//////////////////////////////////////////////////////////////////////////
	///////    passward  추가   /////
	//////////////////////////////////////////////////////////////////////////
	
	int key_pad = 0;
	int old_key_pad=0;
	DDRF = 0x0F;
	
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	
	
	
	
    while (1) 
    {	
		
		
		
		//////////////////////////////////////////////////////////////////////////
		///////////   passward 추가  ////////////////////
		//////////////////////////////////////////////////////////////////////////
		
		key_pad = read_key();
		
		if(key_pad != old_key_pad)
		{
			
			if (key_pad!=-1)
			{
				key_pad_change(key_pad);
				
				if (input_temp!=0)
				{
					input_key_vlaue[k]=input_temp;
					
					k++;
					if (k>6)
					{
						key_set_0();
						
					}
					
					if (k!=6 && input_temp=='#')
					{
						key_set_0();
						
					}
					
					
				}
				
			}
			printf("passward : %s\n\r",key_password);
			printf("input key : %s\n\r",input_key_vlaue);
			old_key_pad = key_pad;
			
			input_temp='\0';
			printf("%d\r\n",k);
		}
		
		
		if (!strcmp(key_password,input_key_vlaue))//&(servoFlag==0))
		{
			cli();
			clrscr();
			lcd(0,0,"OPEN");
			sei();
			printf("OPEN\r\n");
			key_set_0();
			
			//servoFlag = 1;
			sprintf(senddata,"[KMS_ARV]SERVO@ON\n");
			WIFI_send(senddata);
			printf(senddata); printf("\r");
			PORTA=0x02;
			
		}
		
		if (!strcmp(key_setting,input_key_vlaue))
		{
			servoFlag = 0;
			sprintf(senddata,"[KMS_ARV]SERVO@OFF\n");
			WIFI_send(senddata);
			
			
			passwrad_change();
			key_set_0();
			PORTA=0x00;
			
		}
				
		
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		
		
		if(duty != old_duty)
		{
			ocr0 = (256/100.0) * duty;

			if(ocr0 <= 0)
			{
				bit_clear(TCCR0,COM01); //GPIO PB4
				bit_clear(PORTB,PB4);
			}
			else if(ocr0 >= 256)
			ocr0=255;
			else
			bit_set(TCCR0,COM01); //GPIO PB4 PWM
			
			
			OCR0 = ocr0;
			
			old_duty = duty;
		}
		
		memset(recvdata, 0, sizeof(recvdata));

		if(key != 0) 
		{
			printf("Key %d\r\n",key);
			fndnum = key;
		}
		if(rx0flag)
			;
	
		if(wifi_wait("+IPD","+IPD", 25UL))  //수신포멧 :  +IPD,6:[ID]hello  끝문자 0x0a
		{
			for(i=0;i<5;i++)
			{
				if(!strncmp("+IPD",uart1_rxdata[i],4))
				{
					strcpy(recvdata,uart1_rxdata[i]);
					printf(recvdata); printf("\r");
					uart1_rxdata[i][0]='\0';
					break;
				}
			}
		}



		if(recvdata[0] != 0) {
			pToken = strtok(recvdata,",:[@]");
			i = 0;
			while(pToken != NULL)
			{
				pArray[i] =  pToken;

				if(++i >= ARR_CNT)
					break;
				pToken = strtok(NULL,",:[@]");
			}
			if(i < ARR_CNT)
				pArray[i] = NULL;
	
			sprintf(senddata,"[%s]%s",pArray[2],pArray[3]);
			if(!strncmp(pArray[3]," New con",7))
			{
				serverflag = 1;
				recvdata[0] = 0;
				continue;
			}
			if(!strncmp(pArray[3]," Already logged!",7) ||!strncmp(pArray[3]," Authentication",7) )
			{	
				serverflag = 0;
				connect_flag = 0;
				WIFI_init();
				continue;
			}
			if(!strncmp(pArray[3],"LED",3)) {
				if(!strncmp(pArray[4],"ON",2)) {
					PORTA |= 0x01;
					sprintf(senddata,"[%s]%s@%s",pArray[2],pArray[3],pArray[4]);
				}
				else if(!strncmp(pArray[4],"OFF",3)) {
					PORTA &= ~0x01;
					sprintf(senddata,"[%s]%s@%s",pArray[2],pArray[3],pArray[4]);
				}
			} else if(!strncmp(pArray[3],"DCMOTOR",4)) {
				if(!strncmp(pArray[4],"ON",2)) {
					bit_set(MOTOR_PORT,MOTOR_LEFT_ENABLE);
					sprintf(senddata,"[%s]%s@%s",pArray[2],pArray[3],pArray[4]);
				}
				else if(!strncmp(pArray[4],"OFF",3))
				{
					bit_clear(MOTOR_PORT,MOTOR_LEFT_ENABLE);
					sprintf(senddata,"[%s]%s@%s",pArray[2],pArray[3],pArray[4]);
				}
			}
				else if(!strncmp(pArray[3],"SERVO",4)) {
					if(!strncmp(pArray[4],"ON",2)) {
						OCR1A=PULSE_MAX;
						_delay_ms(ROTATION_DELAY);
						OCR1A=PULSE_MIN;
						_delay_ms(ROTATION_DELAY);
						sprintf(senddata,"[%s]%s@%s",pArray[2],pArray[3],pArray[4]);
					}
					else if(!strncmp(pArray[4],"OFF",3))
					{
						OCR1A=PULSE_MIN;
						_delay_ms(ROTATION_DELAY);
						sprintf(senddata,"[%s]%s@%s",pArray[2],pArray[3],pArray[4]);
					}
			}else if(!strncmp(pArray[3],"GETSTATE",strlen("GETSTATE"))) {
				if(!strncmp(pArray[4],"DEV",3)) {
					sprintf(senddata,"[%s]DEV@%s@%s\n",pArray[2],(PORTA & 0x01)?"ON":"OFF",(PORTA & 0x02)?"ON":"OFF");
				}
			}
			else if(!strncmp(pArray[3],"GETSENSOR",9)) {
				if(pArray[4] != NULL) {
					sensorTime = atoi(pArray[4]);
					sprintf(senddata,"[%s]%s@%s",pArray[2],pArray[3],pArray[4]);
				} else {
					sensorTime = 0;
					pArray[3][strlen(pArray[3])-1] = '\0';
					sprintf(senddata,"[%s]%s@%d@%ld\n",pArray[2],pArray[3],10,systick_sec);
				}
				strcpy(getSensorId,pArray[2]);
			}
			else if(!strncmp(pArray[3],"FND",3))
			{
				fndnum = atoi(pArray[4]);
			}
			else if(!strncmp(pArray[3],"GETCDS",6))		
			{
				sprintf(senddata,"[%s]%lu\r\n",pArray[2],cdsValue); //systick_sec
			}
			else if(!strncmp(pArray[3],"GETLED",6))
			{
				sprintf(senddata,"[%s]%s\r\n",pArray[2],(PORTA & 0x02)?"LEDON":"LEDOFF");
			}
		
			if(serverflag)
				WIFI_send(senddata);
				printf(senddata);

		}
		if(sensorTime!= 0 && !(systick_sec % sensorTime ) && (systick_sec_old != systick_sec))	//2sec
		{
			

			sprintf(senddata,"[%s]SENSOR@%lu@20@30\r\n",getSensorId,systick_sec);
			WIFI_send(senddata);
		}
		if(systick_sec_old != systick_sec)
		{
			
			systick_sec_old = systick_sec;				
		}
	
    }
	
	
	
	return 0;
}
unsigned char key_input(void)			/* input key KEY1 - KEY4 */
{
	static unsigned char key_flag = 0;
	unsigned char key;
	key = PINA & 0xf0;				// any key pressed ?
	if(key == 0x00)					// if no key, check key off
	{ 
		if(key_flag == 0)
			return 0;
		else
		{ 
//			_delay_ms(10);
			key_flag = 0;
			return 0;
		}
	}
	else							// if key input, check continuous key
	{ 
		if(key_flag != 0)			// if continuous key, treat as no key input
			return 0;
		else						// if new key, beep and delay for debounce
		{
//			_delay_ms(10);
			key_flag = 1;
			if(key == 0x10) key = 1;
			else if(key == 0x20) key = 2;
			else if(key == 0x40) key = 3;
			else if(key == 0x80) key = 4;
			return key;
		}
	}
}
void display_fnd(int digit,int val)
{
	int n1000,n100,n10,n1,buff;
	n1000 = val / 1000;
	buff = val % 1000;
	n100 = buff / 100;
	buff = buff % 100;
	n10 = buff /10;
	n1 = buff % 10;
	if(digit == DIGIT1)
		display_digit(DIGIT1,n1000);
	else if(digit == DIGIT2)
		display_digit(DIGIT2,n100);
	else if(digit == DIGIT3)
		display_digit(DIGIT3,n10);
	else if(digit == DIGIT4)	
		display_digit(DIGIT4,n1);	

}

void display_digit(int positon,int number)
{
	FND_DIGIT_PORT |= 0x0f;	//FND_DIGIT_PORT 상위 4bit는 유지,하위4bit digit 모두 off
	FND_DIGIT_PORT &= ~( 0x01 << (positon - 1)); //FND_DIGIT_PORT 상위 4bit는 유지
	FND_PORT = fnd[number];
//	_delay_us(100);
}

int read_key(void)
{
	int key_pad = -1;
	for(int x = 0; x < 4; x++){
		// 해당 열에만 LOW를 출력하고 나머지는 HIGH 출력
		COL_OUT |= 0x0F;
		COL_OUT &= (0x01 << x);
		
		_delay_ms(1);
		
		uint8_t read = ROW_IN >> 4;		// 키 상태가 상위 4비트로 반환
		for(int y = 0; y < 4; y++){
			if(bit_is_set(read, y)){
				key_pad = (y *3) + x+1 ;
			}
		}
	}
	return key_pad;
}

void key_set_0(void)
{
	
	
	for( k=0;k<7;k++)
	{
		input_key_vlaue[k]='\0';
	}
	k=0;
}

void key_pad_change(int value)
{
	switch (value)
	{
		case 1:
		input_temp='1';
		break;
		case 2:
		input_temp='2';
		break;
		case 3:
		input_temp='3';
		break;
		case 4:
		input_temp='4';
		break;
		case 5:
		input_temp='5';
		break;
		case 6:
		input_temp='6';
		break;
		case 7:
		input_temp='7';
		break;
		case 8:
		input_temp='8';
		break;
		case 9:
		input_temp='9';
		break;
		case 10:
		input_temp='*';
		break;
		case 11:
		input_temp='0';
		break;
		case 12:
		input_temp='#';
		break;
		default:
		/* Your code here */
		break;
	}
	
}


void passwrad_change(void)
{
	fflush(stdin);
	
	int key_pad = 0;
	int old_key_pad=0;
	
	char old_key_password[7]={0};
	strcpy(old_key_password,key_password);
	printf("KEY change\r\n");
	
	int j=1;
	
	
	while(1)
	{
		
		key_pad = read_key();
		
		
		
		if(key_pad != old_key_pad)
		{
			
			if (key_pad!=-1)
			{
				key_pad_change(key_pad);
				
				if (input_temp!=0 && input_temp!='#')
				{
					key_password[j]=input_temp;
					
					j++;
					
					if (j>=5)
					{
						break;
					}
					
					printf("old key : %s\n\r",old_key_password);
					printf("new key : %s\n\r",key_password);
					
					input_temp=0;
				}
				
			}
			
			old_key_pad = key_pad;
		}
	}
	
}
