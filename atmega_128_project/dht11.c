/* Fast DHT Lirary
 *
 * Copyright (C) 2015 Sergey Denisov.
 * Written by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version 3
 * of the Licence, or (at your option) any later version.
 *
 * Original library written by Adafruit Industries. MIT license.
 */
#define F_CPU 14745600UL		/* Define CPU Frequency e.g. here its Ext. 16MHz */
#include "dht11.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define DHT_COUNT 6
#define DHT_MAXTIMINGS 85


void DHT_Init(struct dht11 *dht, uint8_t pin)  //DDR�� PORT ����
{
    dht->pin = pin;    //(dht_init(dhtInfo,PB3);
    /* Setup the pins! */
    DDR_DHT &= ~(1 << dht->pin);  
	// PB3 ��Ʈ�����ϸ� 0000 1000 , �������ϸ� 1111 01111, 
	//������ ���� �ص忬�� �ϸ� xxxx 0xxx,
	// 0�� �ִ� �ڸ� ���� �Է����� �ϰڴ�
    PORT_DHT |= (1 << dht->pin);     //PB3 ��Ʈ����� or���� �ϸ� xxxx 1xxx
	_delay_ms(1000);  

}

uint8_t DHT_Read(struct dht11 *dht)
{
    uint8_t tmp;
    uint8_t sum = 0;
    uint8_t j = 0, i;
    uint8_t last_state = 1;
    uint16_t counter = 0;
     /*
     * Pull the pin 1 and wait 250 milliseconds
     */
    PORT_DHT |= (1 << dht->pin);     //PB3 ��Ʈ����� or���� �ϸ� xxxx 1xxx
    _delay_ms(250);

//    dht->data[0] = dht->data[1] = dht->data[2] = dht->data[3] = dht->data[4] = 0;  
	//data�迭 0���� �ʱ�ȭ 

    /* Now pull it low for ~20 milliseconds */
    DDR_DHT |= (1 << dht->pin);      					//xxxx 1xxx
    PORT_DHT &= ~(1 << dht->pin);    					//xxxx 0xxx
    _delay_ms(20);
    cli();                           					//���ͷ�Ʈ ����
    PORT_DHT |= (1 << dht->pin);     					//xxxx 1xxx
    _delay_us(40);                   					//40 ���� ������ ������
    DDR_DHT &= ~(1 << dht->pin);     					//xxxx 1xxx

    /* Read the timings */
    for (i = 0; i < DHT_MAXTIMINGS; i++) {             //DHT_MAXTIMINGS = 85
        counter = 0;
        while (1) {
            tmp = ((PIN_DHT & (1 << dht->pin)) >> 1);  //0000 0x00
            _delay_us(3);

            if (tmp != last_state)    //last state = 1
                break;

            counter++;
            _delay_us(1);

            if (counter == 255)  //85 times
                break;
        }

        last_state = ((PIN_DHT & (1 << dht->pin)) >> 1);  //0000 0x00

        if (counter == 255)  //85 times
            break;

        /* Ignore first 3 transitions */
        if ((i >= 4) && (i % 2 == 0)) {
            /* Shove each bit into the storage bytes */
            dht->data[j/8] <<= 1;
            if (counter > DHT_COUNT)  //DHT_COUNT 6
                dht->data[j/8] |= 1;
            j++;
        }
    }

    sei();  		//���ͷ�Ʈ Ȱ��ȭ
    sum = dht->data[0] + dht->data[1] + dht->data[2] + dht->data[3];
	//sum = check sum

    if ((j >= 40) && (dht->data[4] == (sum & 0xFF)))  //data[4]�� check sum
        return 1;
    return 0;
}
