
#define F_CPU 14745600U
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

//#define TLCD_RS  PORTB.0
//#define TLCD_RW  PORTB.1
//#define TLCD_E   PORTB.2
#define TLCD_RS  PG0    //0                 // LCD RS 신호
#define TLCD_RW  PG1    //1              // LCD RW 신호
#define TLCD_E   PG2    //2                // LCD E 신호
#define DATA  PORTC 

#define ON      1
#define OFF     2

#define RIGHT   1
#define LEFT    2
#define bit_set(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#define bit_clear(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
  
void Port_Init(void); void E_Pulse(void);  void Func_Set(void); 
void TLCD_DATA(unsigned char); void Init_LCD(void);  
void lcd_disp(char, char); void clrscr(void); void move_disp(char ); 
void disp_ON_OFF(char, char, char); void lcd( char, char, char *);

void Port_Init(void)
{
  DDRG = 0x0f ;
  DDRC = 0xff; 
}

void E_Pulse(void)
{
//  TLCD_E = 1;
  bit_set(PORTG,TLCD_E);  
  _delay_ms(5);
//  TLCD_E = 0;
  bit_clear(PORTG,TLCD_E);
} 
 
void TLCD_DATA(unsigned char data)
{
 DATA = data ;   
// TLCD_EN;
 bit_clear(PORTG,TLCD_E);
 bit_set(PORTG,TLCD_E);
} 
 
void Func_Set(void)
{
//  TLCD_RW = 0;
//  TLCD_RS = 0;
  bit_clear(PORTG,TLCD_RW);
  bit_clear(PORTG,TLCD_RS);
  TLCD_DATA(0x38);
  E_Pulse();
}


void Init_LCD(void)
{

//  TLCD_E = 0;
  bit_clear(PORTG,TLCD_E);
  _delay_ms(15);
  Func_Set();
  _delay_ms(10); 
  Func_Set();
  _delay_us(150);
  Func_Set();
  
  TLCD_DATA(0x0f);
  E_Pulse();
  TLCD_DATA(0x06);
  E_Pulse();
  TLCD_DATA(0x01);
  E_Pulse();
  
  }        
      
  void lcd_char(char s)
  {
//   TLCD_RS = 1;
   bit_set(PORTG,TLCD_RS);
   TLCD_DATA(s);
   E_Pulse();
  }         
  
void lcd_disp(char x, char y)
  {
//   TLCD_RS = 0 ;
//   TLCD_RW = 0 ;
   bit_clear(PORTG,TLCD_RW);
   bit_clear(PORTG,TLCD_RS);
   if(y==0) TLCD_DATA(x+0x80);
   else if(y==1) TLCD_DATA(x+0xc0);  
   E_Pulse();
   }
 
void move_disp(char p )
 {
//  TLCD_RS = 0;
//  TLCD_RW = 0;
  bit_clear(PORTG,TLCD_RW);
  bit_clear(PORTG,TLCD_RS);
  if(p==LEFT ) TLCD_DATA(0x18);
  else if(p==RIGHT) TLCD_DATA(0x1c);

  E_Pulse();
}

void disp_ON_OFF(char d, char c, char b)
{
   char disp = 0x08;
   
//   TLCD_RS = 0;
//   TLCD_RW = 0 ;
   bit_clear(PORTG,TLCD_RW);
   bit_clear(PORTG,TLCD_RS);
     
   if (d==ON) d = 0x04;
   else       d = 0x00;
   
   if(c == ON) c = 0x02;
   else        c = 0x00;
   
   if(b == ON) b = 0x01;
   else        b = 0x00;
   
   TLCD_DATA(disp | d | c| b);
   E_Pulse();
            
}   
 
void clrscr(void)
{
//  TLCD_RS = 0;
//  TLCD_RW = 0;
  bit_clear(PORTG,TLCD_RW);
  bit_clear(PORTG,TLCD_RS);
  TLCD_DATA(0x01);
  E_Pulse();
 
  _delay_ms(10);
} 


 /***************************************************************
    LCD 에 위치를 지정하여 문자열을 출력할 수 있도록 만든 함수이다 
 ****************************************************************/
void lcd(char x, char y, char *str)
{
  lcd_disp(x, y);
  while(*str) lcd_char(*str++);
}
