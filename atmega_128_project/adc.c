/*
 * adc.c
 *
 * Created: 2019-09-29 오후 2:47:18
 *  Author: shkim
 */ 
#define F_CPU 16000000UL		/* Define CPU Frequency e.g. here its Ext. 16MHz */
#include <avr/io.h>			/* Include AVR std. library file */
#include <avr/interrupt.h>		/* Include avr interrupt header file */
#include <util/delay.h>
volatile int cdsValue;
volatile int adc0Flag;
void CDS_init(void)  	//ADC1 조도 센서 시작
{
	DDRF &= 0xfe;  		//PF0 입력
	ADCSRA = 0x9c; 		//ADC enable, 500kHz,ADC 인터럽트 설정 // ADC enable, 500kHz
	ADMUX = 0x40;
	_delay_us(150); 		// select ADC0(PF0) with AVCC
}
void CDS_start()
{
	ADMUX = 0x40;				//CDS
	ADCSRA |= 0x40;				//ADC Star
}
ISR(ADC_vect)
{
	if(ADMUX == 0x40) 	//CDS
	{
		cdsValue =  ADCL + ADCH*256;
		adc0Flag = 1;
	}
}