/*
 * lcd.h
 *
 * Created: 2020-05-12 오전 9:49:24
 *  Author: admin
 */ 




#ifndef LCD_H_
#define LCD_H_

#define TLCD_RS  PG0   //0                 // LCD RS 신호
#define TLCD_RW  PG1    //1               // LCD RW 신호
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




#endif /* LCD_H_ */