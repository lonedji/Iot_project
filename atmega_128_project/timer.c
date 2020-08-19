/*
 * timer.c
 *
 * Created: 2018-12-07 오후 2:28:58
 *  Author: auto
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
volatile unsigned long systick_count;
volatile unsigned long systick_sec;
volatile unsigned long systick_count1;
volatile unsigned long systick_sec1;
extern volatile int fndnum;
ISR(TIMER0_COMP_vect)				/* OC0 interrupt function */
{
	static int fnd_digit=1;
	systick_count++;		//17.36us * 230 = 4ms
	if(!(systick_count%250))                      // 250th interrupt ?
	{
		systick_sec++;
//		PORTA ^= 0x01;
	}
//	if(!(systick_count%1))
//	{
//		display_fnd(fnd_digit,systick_sec);
		display_fnd(fnd_digit,fndnum);
		if(fnd_digit == 4)
			fnd_digit = 1;
		else
			fnd_digit++;

//		PORTA ^= 0x02;	
//	}
}
ISR(TIMER0_OVF_vect)				/* OC0 interrupt function */
//분주비 128 : 8.68us * 256 = 2.22ms  F = 1/2.22ms = 450Hz
{
	static int systick = 0;
	static int fnd_digit=1;
	systick++;
	if(systick % 2){
		systick_count++;
	}
	if(!(systick%450))                      // 250th interrupt ?
	{
		systick_sec++;
		//		PORTA ^= 0x01;
	}
	//	if(!(systick_count%1))
	//	{
	//		display_fnd(fnd_digit,systick_sec);
	display_fnd(fnd_digit,fndnum);
	if(fnd_digit == 4)
		fnd_digit = 1;
	else
		fnd_digit++;

	//		PORTA ^= 0x02;
	//	}
}

void TIMER0_init()
{
    TCCR0 = _BV(CS02)| _BV(CS00) | _BV(WGM01) | _BV(WGM00) | _BV(COM01);
//	Tt0 = 14745600 / 128 = 67.8nS * 128 = 8.68us

	ASSR = 0x00;                                  // use internal clock
//	OCR0 = 249;                                   // 16MHz/256/(1+249) = 250Hz
//	OCR0 = 229;                                   // 14745600Hz/256/(230) = 250Hz
	TIMSK = 0x01;                                 // enable OVF interrupt
	TIFR = 0xFF;
	TCNT0 = 0x00;                                 // clear Timer/Counter0
}
