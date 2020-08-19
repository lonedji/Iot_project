
#ifndef __DHT11_H__
#define __DHT11_H__

#include <stdint.h>

/*
 * Sensor's port
 */
#define DDR_DHT DDRB
#define PORT_DHT PORTB
#define PIN_DHT PINB

struct dht11 {
    uint8_t data[5];    /* data from sensor store here */
    uint8_t pin;        /* DDR & PORT pin */
};

/**
 * Init dht sensor
 * @dht: sensor struct
 * @pin: PORT & DDR pin
 */
void DHT_Init(struct dht11 *dht, uint8_t pin);
uint8_t DHT_Read(struct dht11 *dht);

#endif
